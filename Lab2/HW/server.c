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
    server.sin_port = htons(5678);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);

    char buf[256];
    int readSize = 0;

    while (readSize = recv(csock, buf, sizeof(buf), 0) > 0) {
        printf("Recv1: %s\n", buf);

        char strA[100];
        strcpy(strA, buf);

        FD_ZERO(&rfds);
        FD_SET(csock, &rfds);
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        retval = select(csock + 1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            perror("Select error!");
        } else if (retval) {
            readSize = recv(csock, buf, sizeof(buf), 0);
            buf[readSize] = '\0';
            printf("Recv2: %s\n", buf);

            char strB[100];
            strcpy(strB, buf);

            sprintf(buf, "%s and %s", strA, strB);
        } else {
            sprintf(buf, "%s Only", strA);
            printf("Recv2: NULL\n");
        }
        printf("Send: %s\n", buf);

        send(csock, buf, sizeof(buf), 0);
    }

    close(sock);

    return 0;
}
