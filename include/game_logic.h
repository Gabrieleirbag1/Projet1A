/**
 * @file game_logic.h
 * @brief This file contains the declarations of functions related to game logic.
 */

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

/**
 * @brief Sends a message with the given button label.
 *
 * @param button_label The label of the button.
 */
void send_message(const char *button_label);

/**
 * @brief Converts a move array to a string representation.
 *
 * @param move Array containing the move coordinates.
 * @param pos String to store the position.
 */
void convert_to_string(int move[3], char pos[]);

/**
 * @brief Resolves the position string into x and y coordinates.
 *
 * @param pos The position string.
 * @param x Pointer to store the x coordinate.
 * @param y Pointer to store the y coordinate.
 * @return 0 if successful, -1 otherwise.
 */
int resolve_pos(const char *pos, int *x, int *y);

/**
 * @brief Selects a square on the game board based on the given position.
 *
 * @param game Pointer to the Game structure.
 * @param pos The position string.
 * @param count Pointer to store the count of selected squares.
 * @return 0 if successful, -1 otherwise.
 */
int select_square(Game *game, const char *pos, int *count);

/**
 * @brief Verifies if the position is valid on the game board.
 *
 * @param game Pointer to the Game structure.
 * @param row Row index.
 * @param col Column index.
 * @return true if the position is valid, false otherwise.
 */
bool verify_position(Game *game, int row, int col);

/**
 * @brief Counts the affected squares in a specified range on the game board.
 *
 * @param game Pointer to the Game structure.
 * @param row_range_start Start row index.
 * @param row_range_end End row index.
 * @param col_range_start Start column index.
 * @param col_range_end End column index.
 * @return Number of affected squares.
 */
int count_affected_squares(Game *game, int row_range_start, int row_range_end, int col_range_start, int col_range_end);

/**
 * @brief Counts the available cells (non-broken squares) on the game board.
 *
 * @param game Pointer to the Game structure.
 * @return Number of available cells.
 */
int count_available_cells(Game *game);

/**
 * @brief Makes a move on the game board.
 *
 * @param game Pointer to the Game structure.
 * @param row Row index.
 * @param col Column index.
 * @param from_history Flag indicating if the move is from history.
 * @param display Flag indicating if the move should be displayed.
 * @return true if the move is successful, false otherwise.
 */
bool make_move(Game *game, int row, int col, bool from_history, bool display);

/**
 * @brief Counts the number of squares on the game board.
 *
 * @param game Pointer to the Game structure.
 * @return The count of squares.
 */
int count_squares(Game *game);

/**
 * @brief Resets the game board.
 *
 * @param game Pointer to the Game structure.
 */
void reset_board(Game *game);

/**
 * @brief Undoes the last move(s) on the game board.
 *
 * @param iterations Number of moves to undo.
 */
void undo_move(int iterations);

/**
 * @brief Gets the available moves on the game board.
 *
 * @param available_moves Array to store available moves.
 * @param available_count Pointer to store the count of available moves.
 */
void get_available_moves(int available_moves[100][3], int *available_count);

/**
 * @brief Manages the game based on the given button label.
 *
 * @param game Pointer to the Game structure.
 * @param button_label The label of the button.
 * @return 0 if successful, -1 otherwise.
 */
int manage_game(Game *game, const char *button_label);

/**
 * @brief Converts the message to uppercase.
 *
 * @param message The message to convert.
 * @return The converted message.
 */
char *convert_uppercase(char *message);

#endif /* #define GAME_LOGIC_H */