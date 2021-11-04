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
    char sendBuf[256];
    char readBuf[256];
    pid_t pid;

    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5675);

    sock = socket(PF_INET, SOCK_STREAM, 0);

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    read(sock, readBuf, sizeof(readBuf));
    printf("%s\n\n", readBuf);
    if (strncmp(readBuf, "Server is full!", 14) == 0) {
        close(sock);
        return 0;
    }
    pid = fork();
    int accArr[3] = {0};

    while (1) {
        if (pid == -1) {
            perror("fork");
            break;
        } else if (pid == 0) {
            read(sock, readBuf, sizeof(readBuf));
            int sendFrom = readBuf[0] - '0';
            char acc = -1;
            if (accArr[sendFrom] == 0) {
                printf("Accept user%d (Y/n): \n", sendFrom);
                scanf(" %c", &acc);
                if(acc == 'Y') accArr[sendFrom] = 1;
            } 
            if (accArr[sendFrom]){
                printf("<User%d> said %s\n", readBuf + 1);
            }
            printf("%s\n", readBuf);
        } else if (pid > 0) {
            if (fgets(sendBuf, sizeof(sendBuf), stdin) != EOF) {
                for (int i = 0; i < 256; i++) {
                    if (sendBuf[i] == '\n') {
                        sendBuf[i] = '\0';
                        break;
                    }
                }
                write(sock, sendBuf, sizeof(sendBuf));
            } else {
                printf("EOF!!\n");
                kill(pid, SIGKILL);
                break;
            }
        }
    }
    close(sock);
    printf("Client Close\n");
    return 0;
}
