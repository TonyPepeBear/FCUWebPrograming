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
    server.sin_port = htons(5677);
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
    int number = working - 1;
    char buf[256];

    if (working >= 5) {
        sprintf(buf, "Server is full!");
        write(csock, buf, sizeof(buf));
        close(csock);
        working--;
        pthread_exit(0);
    }

    char sendBuf[256];
    sprintf(sendBuf, "You are Number %d\n", number);
    write(csock, sendBuf, sizeof(sendBuf));

    while ((readSize = read(csock, buf, sizeof(buf))) != 0) {
        if (sockList[number] == 0) {
            sockList[number] = csock;
            // printf("[%d] %s\n", number);
        }
        buf[readSize] = '\0';
        //printf("CSock ID: %d\n", csock);

        //printf("%d want to SEND message to: %s\n", csock, buf);
        if (strncmp(buf, "ALL", 3) != 0) {
            int sendto = atoi(buf);
            readSize = read(csock, buf, sizeof(buf));
            buf[readSize] = '\0';
            sprintf(sendBuf, "%d SEND message to you: %s", csock, buf);
            printf("%d SEND message to %d: %s", csock, sendto, buf);
            for (int i = 0; i < 100; i++) {
                if (sockList[i] == sendto) {
                    write(sockList[i], sendBuf, sizeof(sendBuf));
                }
            }
        } else {
            readSize = read(csock, buf, sizeof(buf));
            buf[readSize] = '\0';
            sprintf(sendBuf, "%d SEND message to ALL: %s", csock, buf);
            printf("%d SEND message to ALL: %s", csock, buf);
            for (int i = 0; i < 100; i++) {
                if (sockList[i] != 0) {
                    write(sockList[i], sendBuf, sizeof(sendBuf));
                }
            }
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
