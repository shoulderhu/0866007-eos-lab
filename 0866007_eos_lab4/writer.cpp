#include <cstdio>   // fprintf(), snprintf()
#include <cstdlib>  // exit()
#include <cstring>  // strlen()
#include <fcntl.h>  // open()
#include <unistd.h> // sleep(), close()

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./writer <sec>\n");
        exit(EXIT_FAILURE);
    }

    int fd;
    if ((fd = open("/dev/mydev", O_RDWR)) == -1) {
        perror("/dev/mydev");
        exit(EXIT_FAILURE);
    }

    char buf[2] = {};
    int n;
    for (int i = 0 ; i <= strlen(argv[1]) ; i++) {
	n  = argv[1][i];
	printf("%d|", n);
        snprintf(buf, sizeof(buf), "%c", n);
        printf("%s\n", buf);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    close(fd);
    return 0;
}
