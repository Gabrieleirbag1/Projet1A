/**
 * @file utils.h
 * @brief Header file containing utility functions and global variables for the game.
 *
 * This file defines utility functions and global variables used in the game implementation.
 * It includes necessary header files and defines constants for the game board size and button size.
 * It also declares external variables and structures used in the game.
 */

#ifndef GAME_UTILS_H
#define GAME_UTILS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <asm-generic/socket.h>
#include <ctype.h>
#include <gobject/gsignal.h>
#include <X11/Xlib.h>
#include <time.h>
#include <stdbool.h>

#define ROWS 7 /**< Number of rows in the game board. */
#define COLS 9 /**< Number of columns in the game board. */
#define BUTTON_SIZE 90 /**< Size of each button in pixels. */
#define MAXSQUARES 5 /**< Maximum number of squares in a row/column. */

extern char* port; /**< Pointer to the port number for network communication. */
extern char* address; /**< Pointer to the IP address for network communication. */

extern int local; /**< Flag indicating if the game is played locally. */
extern int server; /**< Flag indicating if the game is played as a server. */
extern int client; /**< Flag indicating if the game is played as a client. */
extern int ia; /**< Flag indicating if the game is played against an AI. */

extern int server_fd; /**< File descriptor for the server socket. */
extern int sock; /**< File descriptor for the client socket. */
extern int client_socket; /**< File descriptor for the connected client socket. */

extern char message[1024]; /**< Buffer for storing network messages. */
extern int player; /**< Current player number. */
extern int current_player; /**< Current active player number. */

extern int history_of_moves[100][2];  // Historique des mouvements (on suppose qu'il y aura un maximum de 100 mouvements). 
extern int history_index;

/**
 * @struct Game
 * @brief Structure representing the game board and related widgets.
 *
 * This structure holds the game board as a 2D array of characters.
 * It also stores the GTK buttons associated with each board position.
 * The structure keeps track of the last played position on the board.
 */
typedef struct {
    int board[ROWS][COLS]; /**< 2D array representing the game board. */
    GtkWidget *buttons[ROWS][COLS]; /**< GTK buttons associated with each board position. */
    int last_x; /**< X-coordinate of the last played position. */
    int last_y; /**< Y-coordinate of the last played position. */
} Game;

extern Game game; /**< Global instance of the Game structure. */
extern GtkWidget *turn; /**< GTK widget displaying the current player's turn. */

#endif // GAME_UTILS_H