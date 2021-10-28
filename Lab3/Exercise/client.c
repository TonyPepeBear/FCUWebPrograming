#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct sockaddr_in server;
    int sock, readSize;
    char buf[] = "Hello";
    char sendBuf[256];
    char readBuf[256];
    pid_t pid;

    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5677);

    sock = socket(PF_INET, SOCK_STREAM, 0);

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    write(sock, buf, strlen(buf));
    pid = fork();

    while (1) {
        if (pid == -1) {
            perror("fork");
            break;
        }
        else if (pid == 0) {
            read(sock, readBuf, sizeof(readBuf));
            printf("Rsock ID: %d\n", sock);
            printf("Read Message: %s\n\n", readBuf);
        } else if (pid > 0) {
            if (scanf("%27[^\n]%*c", sendBuf) != EOF) {
                write(sock, sendBuf, strlen(sendBuf));
                printf("WSock ID: %d\n", sock);
                printf("Send Message: %s\n\n", sendBuf);
            } else {
                printf("EOF!!\n");
                kill(pid, SIGKILL);
                break;
            }
        }
    }
    close(sock);
    printf("Client Close");
    return 0;
}
