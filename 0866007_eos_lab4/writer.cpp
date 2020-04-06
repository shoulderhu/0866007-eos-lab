#include <cstdio>   // fprintf(), snprintf()
#include <cstdlib>  // exit()
#include <cstring>  // strlen()
#include <fcntl.h>  // open()
#include <unistd.h> // sleep(), close()

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "./writer <sec>\n");
        exit(EXIT_FAILURE);
    }

    int fd;
    if ((fd = open("/dev/mydev", O_RDWR)) == -1) {
        perror("/dev/mydev");
        exit(EXIT_FAILURE);
    }

    char buf[16] = {};
    for (int i = atoi(argv[1]); i >= 0; --i) {
        snprintf(buf, sizeof(buf), "%d", i);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    close(fd);
    return 0;
}