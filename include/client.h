/**
 * @file client.h
 * @brief Header file for the client module.
 *
 * This file contains the declarations of functions related to the client module.
 * The client module is responsible for handling client-side operations such as
 * sending and receiving messages, setting up the client, and handling disconnection popups.
 */

#ifndef CLIENT_H
#define CLIENT_H

/**
 * @brief Displays a disconnect popup.
 *
 * This function is responsible for displaying a popup to the user when the client
 * gets disconnected from the server.
 */
void disconnect_popup();

/**
 * @brief Sends a message from the client.
 *
 * This function is responsible for sending a message from the client to the server.
 *
 * @param arg The argument passed to the thread function.
 * @return A pointer to the thread function's return value.
 */
void *send_message_client(void *arg);

/**
 * @brief Receives messages on the client.
 *
 * This function is responsible for receiving messages on the client from the server.
 *
 * @param arg The argument passed to the thread function.
 * @return A pointer to the thread function's return value.
 */
void *receive_messages(void *arg);

/**
 * @brief Sets up the client.
 *
 * This function is responsible for setting up the client by initializing necessary
 * resources and establishing a connection with the server.
 *
 * @return The status code indicating the success or failure of the setup process.
 */
int setup_client();

#endif // CLIENT_H