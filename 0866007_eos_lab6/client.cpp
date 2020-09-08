#include <cstdio>        // fprintf(), perror(), sprintf()
#include <cstdlib>       // exit(), atoi()
#include <cstring>       // memset()
#include <unistd.h>      // write(), sleep()
#include <sys/socket.h>  // socket(), connect()
#include <netinet/in.h>  // sockaddr_in
#include <arpa/inet.h>   // inet_addr()

int create_client_socket(const char *ip, const char *port) {
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = inet_addr(ip);
    cli_addr.sin_port = htons((unsigned short)atoi(port));

    if (connect(fd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) == -1) {
        perror("connect()");
        close(fd);
        exit(EXIT_FAILURE);
    }

    return fd;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <ip> <port> <deposit | withdraw> <amount> <time>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = create_client_socket(argv[1], argv[2]);
    int times = atoi(argv[5]);
    char buf[16];

    sprintf(buf, "%s %s\n", argv[3], argv[4]);

    for (int i = 0; i < times; ++i) {
        write(sockfd, buf, strlen(buf));
    }

    close(sockfd);
    return 0;
}
