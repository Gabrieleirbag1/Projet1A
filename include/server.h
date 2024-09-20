/**
 * @file server.h
 * @brief Header file for the server module.
 * 
 * This file contains the declarations of functions related to the server module.
 * The server module is responsible for handling client connections, sending messages, and setting up the server.
 * 
 * @note This file should be included in the main program file.
 */

#ifndef SERVER_H
#define SERVER_H

/**
 * @brief Signal handler for SIGINT.
 * 
 * This function is called when the SIGINT signal is received.
 * It is responsible for handling the termination of the server.
 * 
 * @param sig The signal number.
 */
void handle_sigint(int sig);

/**
 * @brief Function for sending messages from the server.
 * 
 * This function is responsible for sending messages to all connected clients.
 * It runs in a separate thread to allow concurrent message sending.
 * 
 * @param arg The argument passed to the thread function (not used).
 * @return void* The return value of the thread function (not used).
 */
void *send_message_server(void *arg);

/**
 * @brief Function for handling client connections.
 * 
 * This function is responsible for handling client connections.
 * It runs in a separate thread for each connected client.
 * 
 * @param arg The argument passed to the thread function (not used).
 * @return void* The return value of the thread function (not used).
 */
void *client_handler(void *arg);

/**
 * @brief Function for running the server.
 * 
 * This function is responsible for running the server.
 * It creates a separate thread for handling client connections and sending messages.
 * 
 * @param arg The argument passed to the thread function (not used).
 * @return void* The return value of the thread function (not used).
 */
void *server_thread(void *arg);

/**
 * @brief Function for setting up the server.
 * 
 * This function is responsible for setting up the server.
 * It initializes the necessary data structures and starts the server thread.
 */
void setup_server();

#endif // SERVER_H