#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define BOARD_SIZE 3

char board[BOARD_SIZE][BOARD_SIZE];
struct sockaddr_in client_addresses[2];
int current_player = 0;

void initialize_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

void send_board(int sockfd, struct sockaddr_in client_addr) {
    char buffer[1024];
    int len = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            buffer[len++] = board[i][j];
        }
    }

    sendto(sockfd, buffer, len, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
}

int check_winner(char player) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return 1;
        }
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return 1;
        }
    }

    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return 1;
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return 1;
    }

    return 0;
}

int is_board_full() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                return 0;
            }
        }
    }
    return 1;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro ao criar o soquete");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao vincular o soquete");
        exit(1);
    }

    initialize_board();

    printf("Servidor esperando por jogadores...\n");

    while (1) {
        struct sockaddr_in client_addr;
        char buffer[1024];
        int len;

        len = sizeof(client_addr);
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &len);

        client_addresses[current_player] = client_addr;

        // Processar a jogada do jogador e atualizar o tabuleiro
        int row, col;
        sscanf(buffer, "%d %d", &row, &col);

        if (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == ' ') {
            board[row][col] = (current_player == 0) ? 'X' : 'O';
        }

        // Verificar se alguém venceu ou se o jogo empatou
        if (check_winner('X')) {
            printf("Jogador 1 (X) venceu!\n");
            break;
        } else if (check_winner('O')) {
            printf("Jogador 2 (O) venceu!\n");
            break;
        } else if (is_board_full()) {
            printf("O jogo empatou!\n");
            break;
        }

        // Enviar o tabuleiro de volta para o jogador
        send_board(sockfd, client_addresses[current_player]);

        // Alternar para o próximo jogador
        current_player = 1 - current_player;
    }

    close(sockfd);

    return 0;
}