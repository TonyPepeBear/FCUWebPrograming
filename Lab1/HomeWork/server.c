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
    server.sin_port = htons(5678);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);

    readSize = recv(csock, buf, sizeof(buf), 0);
    while (readSize > 0) {
        buf[readSize] = '\0';
        int nums[5];
        sscanf(buf, " %d %d %d %d %d", &nums[0], &nums[1], &nums[2], &nums[3],
               &nums[4]);
        int sum = 0;
        for (int i = 0; i < 5; i++) {
            sum += nums[i];
        }

        for (int i = 0; i < 5; i++) {
            for (int j = i; j < 5; j++) {
                if (nums[i] > nums[j]) {
                    int t = nums[i];
                    nums[i] = nums[j];
                    nums[j] = t;
                }
            }
        }

        float mean = sum / 5;

        sprintf(buf, "MEAN: %f, SUM: %d, MEDIAN: %d", mean, sum, nums[2]);

        send(csock, buf, sizeof(buf), 0);
        readSize = recv(csock, buf, sizeof(buf), 0);
    }

    close(sock);
    return 0;
}
