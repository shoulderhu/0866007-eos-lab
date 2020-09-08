#include <cstdio>    // fprintf()
#include <cstdlib>   // exit(), strtol()
#include <cstring>   // memset()
#include <ctime>     // nanosleep()
#include <csignal>   // signal(), sigaction(), kill()
#include <unistd.h>  // sleep(), getpid()
#include <sys/shm.h> // shmget(), shmat(), shmdt(), shmctl()

typedef struct {
    int guess;
    char result[8];
} data;

int shmid, guess;
data *shm;

void sigint_handler(int signum) {
    if (shmdt(shm) == -1) {
        perror("shmdt()");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, nullptr) == -1) {
        perror("shmctl()");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    if (shm->guess == guess) {
        sprintf(shm->result, "bingo");
        printf("[game] Guess %d, bingo\n", shm->guess);
    }
    else if (shm->guess < guess) {
        sprintf(shm->result, "bigger");
        printf("[game] Guess %d, bigger\n", shm->guess);
    }
    else if (shm->guess > guess) {
        sprintf(shm->result, "smaller");
        printf("[game] Guess %d, smaller\n", shm->guess);
    }

    kill(info->si_pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <key> <guess>", argv[0]);
        exit(EXIT_FAILURE);
    }

    guess = strtol(argv[2], nullptr, 10);

    key_t key = strtol(argv[1], nullptr, 10);
    if ((shmid = shmget(key, sizeof(data), IPC_CREAT | 0644)) == -1) {
        perror("shmid()");
        exit(EXIT_FAILURE);
    }

    if ((shm = (data *)shmat(shmid, nullptr, 0)) == (data *)-1) {
        perror("shmat()");
        exit(EXIT_FAILURE);
    }

    printf("[game] Game PID: %d\n", getpid());

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = sigusr1_handler;

    signal(SIGINT, sigint_handler);
    sigaction(SIGUSR1, &action, nullptr);

    shm->guess = -1;
    sprintf(shm->result, "N/A");

    int ret;
    struct timespec req;
    memset(&req, 0, sizeof(struct timespec));
    req.tv_sec = 24 * 60 * 60;
    req.tv_nsec = 0;

    do {
        ret = nanosleep(&req, &req);
    } while (ret);

    return 0;
}