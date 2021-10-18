#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct sockaddr_in server, client;
    fd_set rfds;

    int sock, csock, addressSize;
    int retval;
    struct timeval tv;
    int num1, num2, ans;
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5679);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);

    char buf[256];
    int readSize = 0;

    while (readSize = recv(csock, buf, sizeof(buf), 0) > 0) {
        char sa[50], sb[50], sc[50];

        printf("Recv1: %s\n", buf);

        strcpy(sa, buf);

        FD_ZERO(&rfds);
        FD_SET(csock, &rfds);
        tv.tv_sec = 4;
        tv.tv_usec = 0;

        retval = select(csock + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            perror("Select error!");
        } else if (retval) {
            readSize = recv(csock, buf, sizeof(buf), 0);
            buf[readSize] = '\0';

            printf("Recv2: %s\n", buf);
            strcpy(sb, buf);

            ///////////////
            FD_ZERO(&rfds);
            FD_SET(csock, &rfds);
            tv.tv_sec = 4;
            tv.tv_usec = 0;

            retval = select(csock + 1, &rfds, NULL, NULL, &tv);
            if (retval == -1) {
                perror("Select error!");
            } else if (retval) {
                readSize = recv(csock, buf, sizeof(buf), 0);
                buf[readSize] = '\0';
                printf("Recv3: %s\n", buf);
                strcpy(sc, buf);
            } else {
                strcpy(sc, "NULL");
            }
        } else {
            strcpy(sb, "NULL");
            strcpy(sc, "NULL");
        }
        if (strlen(sa) >= 5) {
            strcpy(sa, "*****");
        }
        if (strlen(sb) >= 5) {
            strcpy(sb, "*****");
        }
        if (strlen(sc) >= 5) {
            strcpy(sc, "*****");
        }

        sprintf(buf, "%s and %s and %s", sc, sb, sa);

        printf("Send: %s\n", buf);

        send(csock, buf, sizeof(buf), 0);
    }

    close(sock);

    return 0;
}
