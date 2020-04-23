#include <cstdio>        // fprintf(), perror()
#include <cstdlib>       // exit(), atoi()
#include <cstring>       // memset(), strtok()
#include <csignal>       // signal()
#include <unistd.h>      // read()
#include <pthread.h>     // pthread_create(), pthread_exit()
#include <sys/sem.h>     // semget(), semctl(), semop()
#include <sys/socket.h>  // socket(), setsockopt(), bind(), listen(), accept()
#include <sys/wait.h>    // waitpid()
#include <netinet/in.h>  // sockaddr_in

int sockfd, semid, account = 0;

int create_server_socket(const char *port) {
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    if ((setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) == -1) {
        perror("setsockopt()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = INADDR_ANY;
    srv_addr.sin_port = htons((unsigned short)atoi(port));

    if (bind(fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(fd, 10) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    return fd;
}

void sigint_handler(int signum) {
    shutdown(sockfd, 2);
    close(sockfd);

    if (semctl(semid, 0, IPC_RMID) == -1) {
        perror("semctl()");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

int P() {
    struct sembuf sop;
    sop.sem_num = 0; // array[0]
    sop.sem_op = -1; // wait
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) {
        perror("semop()");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int V() {
    struct sembuf sop;
    sop.sem_num = 0; // array[0]
    sop.sem_op = 1;  // signal
    sop.sem_flg = 0;

    if (semop(semid, &sop, 1) == -1) {
        perror("semop()");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void *deposit_withdraw(void *arg) {
    int fd = *((int *)arg), amount, i = 0;
    char buf[16] = {}, ch;

    while (read(fd, &ch, 1)) {
        if (ch == '\n') {
            if (strncmp(buf, "deposit", 7) == 0) {
                amount = atoi(strtok(buf + 7, " "));
                P();
                account += amount;
                printf("After deposit: %d\n", account);
                V();
            } else if (strncmp(buf, "withdraw", 8) == 0) {
                amount = atoi(strtok(buf + 8, " "));
                P();
                account -= amount;
                printf("After withdraw: %d\n", account);
                V();
            } else {
                printf("!deposit and !withdraw\n");
            }

            i = 0;
            memset(buf, 0, sizeof(buf));
        } else { buf[i++] = ch; }
    }

    free(arg);
    pthread_exit(nullptr);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = create_server_socket(argv[1]);
    struct sockaddr_in cli_addr;
    int cli_len = sizeof(cli_addr);

    signal(SIGINT, sigint_handler);

    int key = 123;
    if ((semid = semget(key, 1, IPC_CREAT | 0644)) == -1) {
        perror("semget()");
        exit(EXIT_FAILURE);
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl()");
        exit(EXIT_FAILURE);
    }

    while (true) {
        int *arg = (int *)malloc(sizeof(int));
        if ((*arg = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len)) == -1) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        pthread_t thread;
        if (pthread_create(&thread, nullptr, deposit_withdraw, (void *)arg) != 0) {
            perror("pthread_create()");
            exit(-1);
        }
    }

    return 0;
}
