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

    int sock, addressSize;
    int retval;
    struct timeval tv;
    int num1, num2, ans;
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5678);
    sock = socket(PF_INET, SOCK_DGRAM, 0);

    bind(sock, (struct sockaddr *)&server, sizeof(server));
    tv.tv_sec = 3;
    tv.tv_usec = 0;

    addressSize = sizeof(client);
    while (recvfrom(sock, &num1, sizeof(num1), 0, (struct sockaddr *)&client,
                    &addressSize)) {
        ans = 0;
        printf("Readmessage[1]: %d\n", num1);
        recvfrom(sock, &num2, sizeof(num2), 0, (struct sockaddr *)&client,
                 &addressSize);
        if(num2 == -1) {
            ans = num1 * 100;
            printf("Read Message[2]: NULL\n");
        } else {
            printf("Read Message[2]: %d\n", num2);
            ans = num1 / num2;
        }
        printf("Ans: %d\n", ans);
    
        sendto(sock, &ans, sizeof(ans), 0, (struct sockaddr *) &client, sizeof(client));
    }

    close(sock);

    return 0;
}
