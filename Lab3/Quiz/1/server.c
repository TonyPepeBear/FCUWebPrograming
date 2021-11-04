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
    server.sin_port = htons(5674);
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

    // sprintf(sendBuf, "You are Number %d\n", number);
    // write(csock, sendBuf, sizeof(sendBuf));

    int isLogin = 0;
    working--;

    while (!isLogin) {
        sprintf(sendBuf, "Enter username: ");
        write(csock, sendBuf, sizeof(sendBuf));
        readSize = read(csock, buf, sizeof(buf));
        if (strcmp(buf, "10") == 0 || strcmp(buf, "20") == 0 ||
            strcmp(buf, "30") == 0 || strcmp(buf, "40") == 0) {
            int t = buf[0] - '1';
            sprintf(sendBuf, "Enter password");
            write(csock, sendBuf, sizeof(sendBuf));
            readSize = read(csock, buf, sizeof(buf));
            int i = -1;
            switch (t + 1) {
            case 1:
                i = strcmp(buf, "AA");
                break;
            case 2:
                i = strcmp(buf, "BB");
                break;
            case 3:
                i = strcmp(buf, "CC");
                break;
            case 4:
                i = strcmp(buf, "DD");
                break;
            }
            if (i == 0) {
                isLogin = 1;
                working++;
                sprintf(sendBuf, "Login success!");
                write(csock, sendBuf, sizeof(sendBuf));
            } else {
                sprintf(sendBuf, "Password incorrect!");
                write(csock, sendBuf, sizeof(sendBuf));
            }
        } else {
            write(csock, "User dose not exist!", sizeof(buf));
        }
    }
    while ((readSize = read(csock, buf, sizeof(buf))) != 0) {
        if (sockList[number] == 0) {
            sockList[number] = csock;
            printf("[%d] %s\n", number, buf);
        }
        buf[readSize] = '\0';
        printf("Read Message: %s\n", buf);
        for (int i = 0; i < 100; i++) {
            if (sockList[i] != 0) {
                write(sockList[i], buf, sizeof(buf));
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
