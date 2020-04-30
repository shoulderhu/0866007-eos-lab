#include <cstdio>     // fprintf()
#include <cstdlib>    // exit()
#include <cstring>    // memset()
#include <ctime>      // nanosleep()
#include <csignal>    // kill(), sigaction()
#include <sys/shm.h>  // shmget(), shmat()
#include <sys/time.h> // setitimer()

typedef struct {
    int guess;
    char result[8];
} data;

int shmid, pid, a = 1, b = 100;
data *shm;

void timer_handler(int signum) {
    shm->guess = (int)((a + b) / 2);
    printf("[game] Guess: %d\n", shm->guess);
    kill(pid, SIGUSR1);
}

void sigusr1_handler(int signum) {
    if (strcmp(shm->result, "smaller") == 0) {
        b = shm->guess - 1;
    }
    else if (strcmp(shm->result, "bigger") == 0){
        a = shm->guess + 1;
    }
    else if (strcmp(shm->result, "bingo") == 0) {
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <key> <pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = strtol(argv[2], nullptr, 10);

    key_t key = strtol(argv[1], nullptr, 10);
    if ((shmid = shmget(key, sizeof(data), 0644)) == -1) {
        perror("shmget()");
        exit(EXIT_FAILURE);
    }

    if ((shm = (data *)shmat(shmid, nullptr, 0)) == (data *)-1) {
        perror("shmat()");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, sigusr1_handler);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &timer_handler;
    sigaction(SIGALRM, &sa, nullptr);

    struct itimerval timer;
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, nullptr);

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