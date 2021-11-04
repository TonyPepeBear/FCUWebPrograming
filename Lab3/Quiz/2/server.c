#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void *connection_handler(void *);
int sockList[100] = {0};
int working = 0;

// gcc -o server.o server.c -lpthread
int main() {
    struct sockaddr_in server, client;
    unsigned int sock, csock, addressSiez;
    char buf[256];
    pthread_t sniffer_thread;

    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5675);
    sock = socket(PF_INET, SOCK_STREAM, 0);

    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);

    while (csock = accept(sock, (struct sockaddr *)&client, &addressSiez)) {
        working++;
        // printf("Online connect: %d\n", working);

        if (pthread_create(&sniffer_thread, 0, connection_handler,
                           (void *)&csock) != 0) {
            perror("Thread creation");
        } else {
            pthread_detach(sniffer_thread);
        }
        if (csock < 0) {
            perror("Csock");
        }
    }
    close(sock);
    printf("[Socket Close]\n");
    return 0;
}

void *connection_handler(void *sock) {
    int csock = *(int *)sock;
    int readSize;
    int number = working;
    char buf[256];

    if (working >= 4) {
        sprintf(buf, "Server is full!");
        write(csock, buf, sizeof(buf));
        close(csock);
        working--;
        pthread_exit(0);
    }
    sockList[number] = csock;
    char sendBuf[256];
    
    sprintf(sendBuf, "Welcome user%d\n", number);
    write(csock, sendBuf, sizeof(sendBuf));

    while ((readSize = read(csock, buf, sizeof(buf))) != 0) {
        // user1,user2:
        // user1:
        // 012345678901
        char *s;
        int sendTo1 = -1, sendTo2 = -1; 
    
        if (buf[5] == ',') {
            sendTo1 = buf[4] - '0';
            sendTo2 = buf[10] - '0';
            s = buf + 12;
        } else if (buf[5] == ':') {
            sendTo1 = buf[4] - '0';
            s = buf + 5;
        }
        printf("s1: %d ----- s2: %d\n", sendTo1, sendTo2);

        sprintf(sendBuf, "%d%s", number + 1, s);
        if (sendTo1 != -1) {
            write(sockList[sendTo1], sendBuf, sizeof(sendBuf));
        }
        if (sendTo2 != -1) {
            write(sockList[sendTo2], sendBuf, sizeof(sendBuf));
        }
    }
    if (readSize == 0) {
        printf("Client Disconnect!\n");
        printf("CSock ID: %d\n", csock);

        int delete;
        working--;
        for (int i = 0; i < 100; i++) {
            if (sockList[i] == csock) {
                delete = i;
            }
        }
        for (int i = delete; i < 100; i++) {
            sockList[i] = sockList[i + 1];
            sockList[99] = 0;
        }
    }
    close(csock);
    printf("Client Close\n\n");
    pthread_exit(0);
}
