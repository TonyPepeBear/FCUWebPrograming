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
    server.sin_port = htons(5677);
    sock = socket(PF_INET, SOCK_STREAM, 0);
    bind(sock, (struct sockaddr *)&server, sizeof(server));
    listen(sock, 5);
    addressSize = sizeof(client);
    csock = accept(sock, (struct sockaddr *)&client, &addressSize);

    readSize = recv(csock, buf, sizeof(buf), 0);
    while (readSize > 0) {
        buf[readSize] = '\0';
        int nums[5];
        printf("Read Message: %s", buf);
        sscanf(buf, " %d %d %d %d %d %d", &nums[0], &nums[1], &nums[2], &nums[3],
               &nums[4], &nums[5]);
        sprintf(buf, "[a1] + [a2]: %d\n[a3] - [a4]: %d\n[a5] * [a6]: %d\n",
                nums[0] + nums[1], nums[2] - nums[3], nums[4] * nums[5]);

        printf("%s", buf);
        send(csock, buf, sizeof(buf), 0);
        readSize = recv(csock, buf, sizeof(buf), 0);
    }

    close(sock);
    return 0;
}
