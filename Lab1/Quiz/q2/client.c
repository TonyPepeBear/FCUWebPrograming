#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct sockaddr_in server;
    int sock;
    char buf[256];
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5677);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    while (fgets(buf, sizeof(buf), stdin)) {
        printf("Send Message: %s", buf);
        send(sock, buf, sizeof(buf), 0);
        recv(sock, buf, sizeof(buf), 0);
        printf("%s\n",buf);
    }

    return 0;
}
