#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    fd_set rfds;
    struct sockaddr_in server;
    struct timeval tv;
    int sock, readSize, addressSize;

    int retval;
    int num1, num2, ans;

    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5678);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    addressSize = sizeof(server);
    char buf[256];
    while (fgets(buf, sizeof(buf), stdin)) {
        for (int i = 0; i < 256; i++)
            if (buf[i] == '\n')
                buf[i] = '\0';
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        send(sock, buf, sizeof(buf), 0);

        retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            perror("Select error!");
        } else if (retval) {
            fgets(buf, sizeof(buf), stdin);
            for (int i = 0; i < 256; i++)
                if (buf[i] == '\n')
                    buf[i] = '\0';
            send(sock, buf, sizeof(buf), 0);

            ///////////////  3
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            retval = select(1, &rfds, NULL, NULL, &tv);

            if (retval == -1) {
                perror("Select error!");
            } else if (retval) {
                fgets(buf, sizeof(buf), stdin);
                for (int i = 0; i < 256; i++)
                    if (buf[i] == '\n')
                        buf[i] = '\0';
                send(sock, buf, sizeof(buf), 0);
            }
        }

        readSize = recv(sock, buf, sizeof(buf), 0);
        buf[readSize] = '\0';
        printf("Read Message: %s\n", buf);
    }
    close(sock);

    return 0;
}
