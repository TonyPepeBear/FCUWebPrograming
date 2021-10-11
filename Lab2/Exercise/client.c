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
    sock = socket(PF_INET, SOCK_DGRAM, 0);

    addressSize = sizeof(server);
    tv.tv_sec = 3;
    tv.tv_usec = 0;

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    while (scanf(" %d", &num1)) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        tv.tv_sec = 3;
        tv.tv_usec = 0;

        sendto(sock, &num1, sizeof(num1), 0, (struct sockaddr *)&server,
               sizeof(server));

        retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval == -1) {
            perror("Select error!");
        } else if (retval) {
            scanf(" %d", &num2);
            sendto(sock, &num2, sizeof(num2), 0, (struct sockaddr *)&server,
                   sizeof(server));
        } else {
            num2 = -1;
            sendto(sock, &num2, sizeof(num2), 0, (struct sockaddr *)&server,
                   sizeof(server));
        }
        readSize = recvfrom(sock, &ans, sizeof(ans), 0,
                            (struct sockaddr *)&server, &addressSize);
        printf("Read Message: %d\n", ans);
    }

    return 0;
}
