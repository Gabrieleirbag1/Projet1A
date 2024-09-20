#include "../include/utils.h"
#include "../include/game.h"
#include "../include/game_logic.h"
#include "../include/game_gtk.h"

void send_message(const char *button_label){
    if (server == 1) {
        send(client_socket, button_label, strlen(button_label), 0);
    }
    else if (client == 1) {
        send(sock, button_label, strlen(button_label), 0);
    }
    log_message("Sent: %s\n", button_label);
}

void convert_to_string(int move[3], char pos[]){
    pos[0] = 'A' + move[1];
    pos[1] = '1' + move[0];
    pos[2] = '\0';
}

int resolve_pos(const char *pos, int *x, int *y) {
    const char *alpha = "ABCDEFGHI";
    const char *num = "1234567";
    
    if (strlen(pos) == 2 && strchr(alpha, pos[0]) && strchr(num, pos[1])) {
        *x = pos[1] - '1';
        *y = pos[0] - 'A';
        if (*x < ROWS && *y < COLS) {
            return 1;  // valid
        }
    }
    return 0;  // invalid
}


bool verify_position(Game *game, int row, int col) {
    if (row < ROWS && col < COLS && game->board[row][col] != 0) {
        return true;  // Valid position
    }
    return false;  // Invalid position
}


// Count the affected squares in a range
int count_affected_squares(Game *game, int row_range_start, int row_range_end, int col_range_start, int col_range_end) {
    int count = 0;
    for (int row = row_range_start; row < ROWS; row++) {
        for (int col = col_range_start; col < COLS; col++) {
            // printf("%d", game->board[row][col]);
            if (game->board[row][col] == 1) {
                count++;
                if (count > MAXSQUARES) {
                    return count;
                }
            }
        }
    }
    return count;
}

// Count the available cells (non-broken squares)
int count_available_cells(Game *game) {
    int count = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            count += game->board[row][col];
        }
    }
    return count;
}

// Make a move on the board
bool make_move(Game *game, int row, int col, bool from_history, bool display) {
    if (verify_position(game, row, col)) {
        if (count_affected_squares(game, row, ROWS, col, COLS) <= MAXSQUARES) {
            if (!from_history) {
                history_of_moves[history_index][0] = row;
                history_of_moves[history_index][1] = col;
                history_index++;
            }
            for (int r = row; r < ROWS; r++) {
                for (int c = col; c < COLS; c++) {
                    if (game->board[r][c] != 0) {
                        game->board[r][c] = 0;
                        if (display){
                            gtk_style_context_add_class(gtk_widget_get_style_context(game->buttons[r][c]), "button-broken");
                            gtk_widget_set_sensitive(game->buttons[r][c], FALSE);
                        }
                    }
                }
            }
            return true;
        }
    }
    return false;
}

int select_square(Game *game, const char *pos, int *count) {
    int x, y;
    if (resolve_pos(pos, &x, &y)) {
        if (game->board[x][y] != 0) {
            *count = 0;  // Reset le compteur
            for (int row = x; row < ROWS; row++) {
                for (int col = y; col < COLS; col++) {
                    if (game->board[row][col] != 0) {
                        (*count)++;  // Incrémente le compteur
                    }
                }
            }
            if (*count > MAXSQUARES) {
                printf("You can't break over 5 squares!\n");
                return 0;
            }

            // Casser les cases
            make_move(game, x, y, false, true);

            // Appel de la fonction pour retirer "button-hover" des cases cassées
            remove_button_hover_from_broken_squares(game);

            // Réinitialiser la couleur de la case précédente
            if (game->last_x != -1 && game->last_y != -1) {
                GtkStyleContext *prev_context = gtk_widget_get_style_context(game->buttons[game->last_x][game->last_y]);
                gtk_style_context_remove_class(prev_context, "last-move"); // Retire la classe de couleur rose
            }

            // Mettre à jour la dernière position
            game->last_x = x;
            game->last_y = y;

            // Mettre à jour la couleur du dernier coup
            GtkStyleContext *context = gtk_widget_get_style_context(game->buttons[x][y]);
            gtk_style_context_add_class(context, "last-move");

            return 1;
        } else {
            printf("This square is already broken!\n");
            return 0;
        }
    } else {
        printf("Position is invalid!\n");
        return 0;
    }
}



void reset_board(Game *game){
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            game->board[i][j] = 1;
        }
    }
}

void undo_move(int iterations) {
    reset_board(&game);
    history_index -= iterations;
    if (history_index < 0) history_index = 0;
    for (int i = 0; i < history_index; i++) {
        make_move(&game, history_of_moves[i][0], history_of_moves[i][1], true, false);
    }
}

void get_available_moves(int available_moves[100][3], int *available_count) {
    *available_count = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int count = count_affected_squares(&game, row, ROWS, col, COLS);
            if (count <= MAXSQUARES && verify_position(&game, row, col)) {
                available_moves[*available_count][0] = row;
                available_moves[*available_count][1] = col;
                available_moves[*available_count][2] = count;
                (*available_count)++;
            }
        }
    }
}

int count_squares(Game *game) {
    int count_squares = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (game->board[i][j] == 1) {
                count_squares++;
            }
        }
    }
    return count_squares;
}

int manage_game(Game *game, const char *button_label) {
    int count = 0; // Compteur de cases

    // Vérifie si le coup est valide et met à jour le plateau
    if (select_square(game, button_label, &count)) {
        print_game(game);
        
        // Vérifie si la case A1 a été cassée
        if (strcmp(button_label, "A1") == 0 && count <= 5) {
            // Envoie un message au serveur ou au client
            show_victory_message(gtk_widget_get_toplevel(game->buttons[0][0])); // Affiche le message de victoire
            gtk_label_set_markup(GTK_LABEL(turn), "\n GAME ENDED ! \n");
            return 0;
        }

        // Changement de joueur
        if (current_player == 1) {
            gtk_label_set_markup(GTK_LABEL(turn), "\n <b>Turn : Player 2</b> \n");
            current_player = 2;
        } else {
            gtk_label_set_markup(GTK_LABEL(turn), "\n <b>Turn : Player 1</b> \n");
            current_player = 1;
        }
        return 0;
    }else{
        return 1;
    }
}

char *convert_uppercase(char *message) {
    for (int i = 0; message[i]; i++) {
        message[i] = toupper(message[i]);
    }
    return message;
}