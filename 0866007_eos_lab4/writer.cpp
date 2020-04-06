#include <cstdio>   // fprintf()
#include <cstdlib>  // exit()
#include <cstring>
#include <fcntl.h>  // open()
#include <unistd.h> // close()

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "./writer <sec>\n");
        exit(EXIT_FAILURE);
    }

    int fd;
    if ((fd = open("/dev/mydev", O_RDWR)) == -1) {
        perror("Open /dev/mydev failed.");
        exit(EXIT_FAILURE);
    }

    int i;
    char buf[8] = {};
    /*
    for (i = 0; ; ++i) {
        i = i % strlen(argv[1]);
        memset(buf, 0, sizeof(buf));
        buf[0] = argv[1][i];
        write(fd, buf, strlen(buf));
        sleep(3);
    }
     */

    close(fd);
    return 0;
}