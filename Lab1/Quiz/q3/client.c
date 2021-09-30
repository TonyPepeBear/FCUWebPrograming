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
    server.sin_port = htons(5679);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    while (fgets(buf, sizeof(buf), stdin)) {
        if(!strcmp(buf, "EXIT\n")) {
            printf("EXIT!!\n");
            break;
        }
        printf("Send Message: %s", buf);
        send(sock, buf, sizeof(buf), 0);
        recv(sock, buf, sizeof(buf), 0);
        printf("%s\n",buf);
    }

    close(sock);

    return 0;
}
