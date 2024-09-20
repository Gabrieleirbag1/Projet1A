#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define ROWS 7
#define COLS 9
#define MAXIMUM 5

int history_of_moves[100][2];  // History to store moves (assuming a maximum of 100 moves)
int history_index = 0;
int board[ROWS][COLS];  // The game board

// Initialize game with all cells set to 1
void init_game() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col] = 1;
        }
    }
}

// Reset the board
void reset_board() {
    init_game();
}

// Set the board to a specific pattern
void set_board(int pattern[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col] = pattern[row][col];
        }
    }
}

// Verify if the position is valid
bool verify_position(int row, int col) {
    if (row < ROWS && col < COLS && board[row][col] != 0) {
        return true;  // Valid position
    }
    return false;  // Invalid position
}

// Count the affected squares in a range
int count_affected_squares(int row_range_start, int row_range_end, int col_range_start, int col_range_end) {
    int count = 0;
    for (int row = row_range_start; row < row_range_end; row++) {
        for (int col = col_range_start; col < col_range_end; col++) {
            if (board[row][col] == 1) {
                count++;
                if (count > MAXIMUM) {
                    return count;
                }
            }
        }
    }
    return count;
}

// Count the available cells (non-broken squares)
int count_available_cells() {
    int count = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            count += board[row][col];
        }
    }
    return count;
}

// Convert position from string to coordinates
bool position_to_coords(char pos[], int *row, int *col) {
    if (pos[0] >= 'A' && pos[0] <= 'I' && pos[1] >= '1' && pos[1] <= '7') {
        *row = pos[1] - '1';
        *col = pos[0] - 'A';
        return true;
    }
    printf("Invalid format\n");
    return false;
}

// Break a square by setting it to 0
void break_square(int row, int col) {
    board[row][col] = 0;
}

// "Unbreak" a square by setting it back to 1 (for undo)
void unbreak_square(int row, int col) {
    board[row][col] = 1;
}

// Make a move on the board
bool make_move(int row, int col, bool from_history) {
    if (verify_position(row, col)) {
        if (count_affected_squares(row, ROWS, col, COLS) <= MAXIMUM) {
            if (!from_history) {
                history_of_moves[history_index][0] = row;
                history_of_moves[history_index][1] = col;
                history_index++;
            }
            for (int r = row; r < ROWS; r++) {
                for (int c = col; c < COLS; c++) {
                    break_square(r, c);
                }
            }
            return true;
        }
    }
    return false;
}

// Undo the last move by "unbreaking" squares
void undo_move(int iterations) {
    reset_board();
    history_index -= iterations;
    if (history_index < 0) history_index = 0;
    for (int i = 0; i < history_index; i++) {
        make_move(history_of_moves[i][0], history_of_moves[i][1], true);
    }
}

// Get available moves on the board
void get_available_moves(int available_moves[100][3], int *available_count) {
    *available_count = 0;
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int count = count_affected_squares(row, ROWS, col, COLS);
            if (count <= MAXIMUM && verify_position(row, col)) {
                available_moves[*available_count][0] = row;
                available_moves[*available_count][1] = col;
                available_moves[*available_count][2] = count;
                (*available_count)++;
            }
        }
    }
}

// Print the board
void print_board() {
    printf("   A  B  C  D  E  F  G  H  I\n");
    for (int row = 0; row < ROWS; row++) {
        printf("%d  ", row + 1);
        for (int col = 0; col < COLS; col++) {
            printf("%d  ", board[row][col]);
        }
        printf("\n");
    }
}

// Minimax algorithm
int minimax(int move[3], int number_of_squares, int value, int depth, bool maximizing_player) {
    if (depth == 0 || number_of_squares == 0) {
        return value - 5;  // End of depth or no squares left
    }

    int evals[100];
    int eval_count = 0;
    make_move(move[0], move[1], false);
    int moves[100][3];
    //if (!maximizing_player) {
    //    value += number_of_squares * 2;  // Favor positions with more squares left for AI
    //} else {
    //    value -= number_of_squares * 2;  // Penalize positions with more squares left for opponent
    //}
    int available_count;
    get_available_moves(moves, &available_count);
    number_of_squares = count_available_cells();

    if (maximizing_player) {
        if (number_of_squares == 0) {
            value -= 7;  // Losing case
        } else {
            for (int i = 0; i < available_count; i++) {
                int evaluation = minimax(moves[i], number_of_squares, value, depth - 1, false);
                evals[eval_count++] = evaluation;
            }
        }
    } else {
        if (number_of_squares == 0) {
            value += 6;
        } else {
            for (int i = 0; i < available_count; i++) {
                int evaluation = minimax(moves[i], number_of_squares, value - 1, depth - 1, true);
                evals[eval_count++] = evaluation;
            }
        }
    }

    undo_move(1);  // Undo move after evaluation

    if (eval_count > 0) {
        int sum = 0;
        for (int i = 0; i < eval_count; i++) {
            sum += evals[i];
        }
        return sum / eval_count;
    } else {
        return value;
    }
}

// Find the best move
void find_best_move(int number_of_squares, int depth, int best_move[3]) {
    // Get all available moves
    int moves[100][3];
    int available_count;
    get_available_moves(moves, &available_count);
    
    if (available_count == 0) {
        return; // No available moves
    }

    int evals[100][2]; // To store moves and their evaluations
    int best_evaluation = INT_MIN; // Set initial best evaluation to the lowest possible
    int best_move_idx = 0;

    // Loop through all available moves and evaluate them
    for (int i = 0; i < available_count; i++) {
        int evaluation = minimax(moves[i], number_of_squares, 0, depth, true);
        evals[i][0] = i; // Store the index of the move
        evals[i][1] = evaluation; // Store the evaluation score
        
        // Compare to find the best move
        if (evaluation > best_evaluation) {
            best_evaluation = evaluation;
            best_move_idx = i;
        } else if (evaluation == best_evaluation) {
            // Tie-breaking: prefer the move that affects fewer squares
            if (moves[i][2] < moves[best_move_idx][2]) {
                best_move_idx = i;
            }
        }
    }

    // Set the best move to the one found
    best_move[0] = moves[best_move_idx][0];
    best_move[1] = moves[best_move_idx][1];
    best_move[2] = moves[best_move_idx][2];
}


// Get the AI's best move
void best_move(int move[3]) {
    printf("AI IS SEARCHING FOR BEST MOVE ...\n");
    int number_of_squares = count_available_cells();

    if (number_of_squares > 20) {
        int moves[100][3];
        int available_count;
        get_available_moves(moves, &available_count);
        int random_index = rand() % available_count;
        move[0] = moves[random_index][0];
        move[1] = moves[random_index][1];
        move[2] = moves[random_index][2];
    } else if (number_of_squares < 12) {
        find_best_move(number_of_squares, 10, move);
    } else {
        find_best_move(number_of_squares, 6, move);
    }
}

int main() {
    // Initialize the game
    init_game();
    
    // Example usage of the functions
    print_board();
    while (count_available_cells() > 0) {
        printf("\n");
        char position[3];
        printf("Insert position (e.g., A1): ");
        scanf("%s", position);

        int row, col;
        while (!position_to_coords(position, &row, &col)) {
            printf("Insert valid position (e.g., A1): ");
            scanf("%s", position);
        }

        make_move(row, col, false);
        print_board();

        printf("\nAI's turn...\n");
        int next_move[3];
        best_move(next_move);
        make_move(next_move[0], next_move[1], false);
        print_board();
    }

    return 0;
}