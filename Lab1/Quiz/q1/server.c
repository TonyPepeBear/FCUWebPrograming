#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    char ip[16];
    int port = 0;
    printf("Please enter the ip: ");
    scanf(" %s", ip);
    printf("Please enter the port: ");
    scanf(" %d", &port);
    while (port >= 2000 && port <= 5000) {
        printf("The port numner is out of range!\n");
        printf("Please enter the port: ");
        scanf(" %d", &port);
    }

    struct sockaddr_in server, client;
    int sock, csock, readSize, addressSize;
    char buf[256];
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(port);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);
    readSize = recv(csock, buf, sizeof(buf), 0);
    buf[readSize] = '\0';
    printf("Read Message: %s\n", buf);
    close(sock);
    return 0;
}
