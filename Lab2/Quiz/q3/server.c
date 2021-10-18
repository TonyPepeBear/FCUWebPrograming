#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

int s2int(char *s) {
    int ans = 0;
    int t = 1;
    for (int i = strlen(s) - 1; i >= 0; i--, t *= 10) {
        ans += (s[i] - '0') * t;
    }
    return ans;
}

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
        char str[50];
        int flag = s2int(buf);
        int repeat = 0;

        printf("Recv1: %s\n", buf);

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
            strcpy(str, buf);

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
                repeat = s2int(buf);
            } else {
                repeat = 3;
            }
        } else {
            strcpy(str, "NULL");
            repeat = 3;
        }

        if (flag == 1) {
            str[0] = toupper(str[0]);
            for (int i = 1; i < strlen(str); i++) {
                str[i] = tolower(str[i]);
            }
        } else if (flag == 2) {
            str[0] = tolower(str[0]);
            for (int i = 1; i < strlen(str); i++) {
                str[i] = toupper(str[i]);
            }
        }

        int len = strlen(str);
        str[len] = ' ';
        str[++len] = '\0';

        for (int i = 0; i < 256; i++) {
            if (i < len * repeat) {
                buf[i] = str[i % len];
            } else
                buf[i] = '\0';
        }

        printf("Send: %s\n", buf);

        send(csock, buf, sizeof(buf), 0);
    }

    close(sock);

    return 0;
}
