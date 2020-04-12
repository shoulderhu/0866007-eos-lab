#include <cstdio>        // fprintf(), perror()
#include <cstdlib>       // exit()
#include <cstring>       // memset()
#include <csignal>       // signal()
#include <unistd.h>      // fork(), getpid(), dup2(), execlp()
#include <sys/socket.h>  // socket(), setsockopt(), bind(), listen(), accept()
#include <sys/wait.h>    // waitpid()
#include <netinet/in.h>  // sockaddr_in

int sockfd;

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
    exit(EXIT_SUCCESS);
}

void sigchld_handler(int signum) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void child_process(int fd) {
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2()");
        exit(EXIT_FAILURE);
    }

    close(fd);
    if (execlp("sl", "sl", "-l", nullptr) == -1) {
        perror("execlp()");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = create_server_socket(argv[1]);
    struct sockaddr_in cli_addr;
    int clifd, cli_len = sizeof(cli_addr);

    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, sigchld_handler);

    while (true) {
        if ((clifd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_len)) == -1) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        int pid = fork();

        switch (pid) {
            case -1:
                perror("fork()");
                exit(EXIT_FAILURE);
            case 0:  // child process
                close(sockfd);
                child_process(clifd);
                break;
            default: // parent process
                printf("Train ID: %d\n", pid);
                close(clifd);
                break;
        }
    }

    return 0;
}
