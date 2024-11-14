#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

struct sending_packet {
    char sender[1024];
    char receiver[1024];
    char nickname[20];  // 추가: 클라이언트의 닉네임
    char msg[1024];
};

void main() {
    struct sockaddr_in s_addr;
    int sock_fd;
    char buffer[1024] = {0};
    char *buf = NULL;
    struct sending_packet pck;
    int check;
    char nickname[20];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd <= 0) {
        perror("socket failed: ");
        exit(1);
    }

    memset(&s_addr, '0', sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(9090);
    check = inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);
    if (check <= 0) {
        perror("inet_pton failed: ");
        exit(1);
    }
    check = connect(sock_fd, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (check < 0) {
        perror("connect failed: ");
        exit(1);
    }

    printf("Input nickname: ");
    scanf("%s", nickname);
    printf("%s has entered.\n", nickname);

    int flag = 0;
    size_t line_len = 0;
    while (1) {
        printf(">>");
        getline(&buf, &line_len, stdin);
        buf[line_len] = '\0';

        // 제비뽑기 기능 추가
        if (strcmp(buf, "draw lots\n") == 0) {
            srand(time(0));
            int lot = rand() % 6 + 1;
            sprintf(pck.msg, "draw lots: %d\n", lot);
        } else {
            sprintf(pck.msg, "%s", buf);
        }

        sprintf(pck.sender, "Client");
        sprintf(pck.receiver, "Server");
        sprintf(pck.nickname, "%s", nickname);  // 추가: 클라이언트의 닉네임

        if (strcmp(pck.msg, "quit\n") == 0) {
            flag = -1;
        }

        send(sock_fd, (struct sending_packet *)&pck, sizeof(pck), 0);
        recv(sock_fd, (struct sending_packet *)&pck, sizeof(pck), 0);
        printf("%s (%s): %s\n", pck.sender, pck.nickname, pck.msg);  // 수정: 닉네임 출력

        if (flag == -1) {
            printf("You pressed 'quit'. Chat break");
            break;
        }
    }
    shutdown(sock_fd, SHUT_WR);
}