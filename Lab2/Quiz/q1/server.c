#include <arpa/inet.h>
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
    server.sin_port = htons(5678);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);

    char buf[256];
    int readSize = 0;

    while (readSize = recv(csock, buf, sizeof(buf), 0) > 0) {
        int a = 0;
        int b = 0;

        printf("Recv1: %s\n", buf);

        a = s2int(buf);

        FD_ZERO(&rfds);
        FD_SET(csock, &rfds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int flag = -1;

        retval = select(csock + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            perror("Select error!");
        } else if (retval) {
            readSize = recv(csock, buf, sizeof(buf), 0);
            buf[readSize] = '\0';
            printf("Recv2: %s\n", buf);

            if (buf[0] == '+') {
                flag = 1;
            } else if (buf[0] == '-') {
                flag = 2;
            } else if (buf[0] == '*') {
                flag = 3;
            } else if (buf[0] == '/') {
                flag = 4;
            } else if (buf[0] == '%') {
                flag = 5;
            }

            ///////////////
            FD_ZERO(&rfds);
            FD_SET(csock, &rfds);
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            retval = select(csock + 1, &rfds, NULL, NULL, &tv);
            if (retval == -1) {
                perror("Select error!");
            } else if (retval) {
                readSize = recv(csock, buf, sizeof(buf), 0);
                buf[readSize] = '\0';
                printf("Recv3: %s\n", buf);
                b = s2int(buf);
            } else {
                flag = 0;
            }
        } else {
            flag = 0;
        }
        if (flag == 0) {
            sprintf(buf, "%d", a * a);
        } else if (flag == 1) {
            sprintf(buf, "%d", a + b);
        } else if (flag == 2) {
            sprintf(buf, "%d", a - b);
        } else if (flag == 3) {
            sprintf(buf, "%d", a * b);
        } else if (flag == 4) {
            sprintf(buf, "%f", (float)a / (float)b);
        } else if (flag == 5) {
            sprintf(buf, "%d", a % b);
        }

        printf("Send: %s\n", buf);

        send(csock, buf, sizeof(buf), 0);
    }

    close(sock);

    return 0;
}
