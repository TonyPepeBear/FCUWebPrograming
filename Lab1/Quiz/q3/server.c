#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    struct sockaddr_in server, client;
    int sock, csock, readSize, addressSize;
    char buf[256];
    bzero(&server, sizeof(server));
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(5679);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);

    readSize = recv(csock, buf, sizeof(buf), 0);
    while (readSize > 0) {
        buf[readSize] = '\0';
        int nums[100] = {0}, count = 0;
        printf("Read Message: %s", buf);
        int len = strlen(buf);
        for (int i = 0, t = 1; i < len; i++) {
            if (buf[i] >= '0' && buf[i] <= '9') {
                // printf("TODO: Delete: %c ", buf[i]);
                nums[count] += (buf[i] - '0') * t;
            } else {
                t = 1;
                count++;
            }
        }

        // printf("\nTODO: DELETE: count: %d :::: ", count);
        // for (int i = 0; i < count; i++) {
        //     printf("%d ", nums[i]);
        // }
        // printf("\n");

        if (count % 2) {
            int max = nums[0], min = nums[0];
            for (int i = 0; i < count; i++) {
                if (max < nums[i])
                    max = nums[i];
                if (min > nums[i])
                    min = nums[i];
            }
            sprintf(buf, "[Max-Min]: %d\n", max - min);
        } else {
            float sum = 0;
            for (int i = 0; i < count; i++) {
                sum += nums[i];
            }
            sprintf(buf, "[Average]: %f\n", sum / count);
        }

        printf("%s", buf);
        send(csock, buf, sizeof(buf), 0);
        readSize = recv(csock, buf, sizeof(buf), 0);
    }

    close(sock);
    return 0;
}
