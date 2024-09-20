#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define ROWS 7
#define COLUMNS 9
#define MAXSQUARES 5

typedef struct {
    char board[ROWS][COLUMNS];
} Game;

void init_game(Game *game) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            game->board[i][j] = '1';
        }
    }
}

void print_game(Game *game) {
    printf("   A  B  C  D  E  F  G  H  I\n");
    for (int i = 0; i < ROWS; i++) {
        printf("%d  ", i + 1);
        for (int j = 0; j < COLUMNS; j++) {
            printf("%c  ", game->board[i][j]);
        }
        printf("\n");
    }
}

int resolve_pos(char *pos, int *x, int *y) {
    char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *num = "123456789";
    char dict[26];
    
    for (int i = 0; i < 26; i++) {
        dict[alpha[i] - 'A'] = i;
    }

    if (strlen(pos) == 2 && strchr(alpha, pos[0]) && strchr(num, pos[1])) {
        *x = (pos[1] - '1');
        *y = dict[pos[0] - 'A'];
        if (*x < ROWS && *y < COLUMNS) {
            return 1;  // valid
        };
    };
    return 0;  // invalid
}

int select_square(Game *game, char *pos) {
    int x, y;
    if (resolve_pos(pos, &x, &y)) {
        if (game->board[x][y] != '0') {
            int count = 0;
            for (int row = x; row < ROWS; row++) {
                for (int col = y; col < COLUMNS && count <= MAXSQUARES; col++) {
                    if (game->board[row][col] != '0') {
                        count++;
                    }
                }
            }
            if (count > MAXSQUARES) {
                printf("You can't break over 5 squares!\n");
                return 0;
            }

            for (int row = x; row < ROWS; row++) {
                for (int col = y; col < COLUMNS; col++) {
                    if (game->board[row][col] != '0') {
                        game->board[row][col] = '0';
                    }
                }
            }
            return 1;
        } else {
            printf("Square is already broken !\n");
            return 0;
        }
    } else {
        printf("Position is invalid !\n");
        return 0;
    }
}

int count_squares(Game *game){
    int count_squares = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++){
            if (game->board[i][j] == '1'){
                count_squares++;
            }
        }
    }
    return count_squares;
}

void gameloop(Game *game) {
    char current_player = 'A';
    char input[3];
    
    while (1) {
        print_game(game);
        printf("IT'S PLAYER %c'S TURN\n", current_player);
        while (1) {
            scanf("%s", input);
            if (select_square(game, input)) {
                break;
            } else {
                print_game(game);
            }
        }
        current_player = (current_player == 'A') ? 'B' : 'A';
        if (count_squares(game) == 0){
            printf("GAME IS OVER PLAYER %c WINS!!!", current_player);
            break;
            return;
        };
    }
}

int main() {
    Game game;
    init_game(&game);
    gameloop(&game);
    return 0;
}
