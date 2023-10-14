#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUF_SIZE 1024

void display_board(char* board) {
    printf("Tabuleiro:\n");
    for (int i = 0; i < 9; i++) {
        printf("%c ", board[i]);
        if ((i + 1) % 3 == 0) {
            printf("\n");
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar o soquete");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    while (1) {
        int row, col;
        display_board(buffer);

        printf("Digite sua jogada (linha coluna): ");
        scanf("%d %d", &row, &col);
        snprintf(buffer, sizeof(buffer), "%d %d", row, col);

        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

        // Receba e exiba o estado atual do tabuleiro
        int len = sizeof(server_addr);
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &len);
        display_board(buffer);
    }

    close(sockfd);

    return 0;
}