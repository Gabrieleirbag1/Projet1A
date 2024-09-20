/**
 * @file game.h
 * @brief Header file for the game module.
 */

#ifndef GAME_H
#define GAME_H

/**
 * @brief Initializes the log files.
 * 
 * This function is responsible for initializing the log files used in the game.
 * It should be called before any logging operations are performed.
 */
void init_log_files();

/**
 * @brief Logs a message to the log file.
 * @param log_file The log file.
 * @param format The format string.
 * @param ... The arguments to format.
 */
void log_message(const char *format, ...);

/**
 * @brief Processes the command line arguments.
 * @param argc The number of arguments.
 * @param argv The argument array.
 * @return 0 if successful, 1 otherwise.
 */
int process_arguments(int argc, char *argv[]);

/**
 * @brief Sets up the network connection.
 */
void setup_network();

/**
 * @brief Closes all open files and sockets.
 */
void close_all();

/**
 * @brief The main function.
 * @param argc The number of command line arguments.
 * @param argv The command line argument array.
 * @return 0 if successful, 1 otherwise.
 */
int main(int argc, char *argv[]);

#endif // GAME_H
