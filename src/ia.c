#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/utils.h"
#include "../include/game.h"
#include "../include/game_logic.h"
#include "../include/game_gtk.h"

// Minimax algorithm
int minimax(Game *game, int move[3], int number_of_squares, int value, int depth, bool maximizing_player) {
    if (depth == 0 || number_of_squares == 0) {
        return value - 5;  // End of depth or no squares left
    }

    int evals[100];
    int eval_count = 0;
    make_move(game, move[0], move[1], false, false);
    int moves[100][3];
    int available_count;
    get_available_moves(moves, &available_count);
    number_of_squares = count_available_cells(game);

    if (maximizing_player) {
        if (number_of_squares == 0) {
            value -= 7;  // Losing case
        } else {
            for (int i = 0; i < available_count; i++) {
                int evaluation = minimax(game, moves[i], number_of_squares, value, depth - 1, false);
                evals[eval_count++] = evaluation;
            }
        }
    } else {
        if (number_of_squares == 0) {
            value += 6;
        } else {
            for (int i = 0; i < available_count; i++) {
                int evaluation = minimax(game, moves[i], number_of_squares, value - 1, depth - 1, true);
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
        int evaluation = minimax(&game, moves[i], number_of_squares, 0, depth, true);
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
    int number_of_squares = count_available_cells(&game);

    if (number_of_squares > 20) {
        int moves[100][3];
        int available_count;
        get_available_moves(moves, &available_count);
        // printf("Available moves:\n");
        // for (int i = 0; i < available_count; i++) {
        //     printf("Move %d: Row %d, Col %d, Affected Squares %d\n", 
        //         i + 1, moves[i][0], moves[i][1], moves[i][2]);
        // }
        int random_index = rand() % available_count;

        move[0] = moves[random_index][0];
        move[1] = moves[random_index][1];
        move[2] = moves[random_index][2];
    } else if (number_of_squares < 12) {
        find_best_move(number_of_squares, 12, move);
    } else {
        find_best_move(number_of_squares, 7, move);
    }
}

// Convert move to string
void ia_move(Game *game) {
    int randint = (rand() % 2) + 1;
    if (local == 0){
        sleep(randint);
    }
    // Get the best move
    int next_move[3];
    best_move(next_move);
    char pos[3];
    // Convert the move to string
    convert_to_string(next_move, pos);
    int status = manage_game(game, pos);
    // Send the move if successful
    if (status == 0 && local == 0){
        send_message(pos);
    }
}