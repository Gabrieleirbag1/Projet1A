/**
 * @file ia.h
 * @brief Header file for the IA module.
 */


#ifndef IA_H
#define IA_H

/**
 * @brief Minimax algorithm for evaluating game moves.
 * 
 * @param game Pointer to the game structure.
 * @param move Array to store the move.
 * @param number_of_squares Number of squares available.
 * @param value Current evaluation value.
 * @param depth Current depth of the search.
 * @param maximizing_player Boolean indicating if the current player is maximizing.
 * @return Evaluation score of the move.
 */
int minimax(Game *game, int move[3], int number_of_squares, int value, int depth, bool maximizing_player);

/**
 * @brief Find the best move using the minimax algorithm.
 * 
 * @param number_of_squares Number of squares available.
 * @param depth Depth of the search.
 * @param best_move Array to store the best move.
 */
void find_best_move(int number_of_squares, int depth, int best_move[3]);

/**
 * @brief Get the AI's best move.
 * 
 * @param move Array to store the best move.
 */
void best_move(int move[3]);

/**
 * @brief Make a move for the AI.
 * 
 * @param game Pointer to the game structure.
 */
void ia_move(Game *game);

#endif // IA_H