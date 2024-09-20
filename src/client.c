#include "../include/utils.h"
#include "../include/game.h"
#include "../include/game_logic.h"
#include "../include/client.h"
#include "../include/ia.h"

// Function to display a popup when the server disconnects
void disconnect_popup() {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Server disconnected");
    gtk_dialog_run(GTK_DIALOG(dialog));
    close_all();
    exit(0);
}

// Thread function to send messages to the server
void *send_message_client(void *arg) {
    char text[1024];

    while (1) {
        // Read input from the user
        fgets(text, 1024, stdin);
        text[strcspn(text, "\n")] = '\0';
        char *up_text = convert_uppercase(text);

        // Only send messages if it's the current player's turn
        if (current_player != player) {
            manage_game(&game, up_text);
            send_message(up_text);
        }
    }

    return NULL;
}

// Thread function to receive messages from the server
void *receive_messages(void *arg) {
    char buffer[1024];
    int valread;

    while (1) {
        // Read message from the server
        valread = read(sock, buffer, 1024);
        if (valread <= 0) {
            log_message("Server lost or read error\n");
            disconnect_popup();
            break;
        }
        log_message("Received: %s\n", buffer);
        buffer[valread] = '\0';

        // Handle server exit message
        if (strcmp(buffer, "exit") == 0) {
            log_message("Client has been closed by server\n");
            sleep(1);
            disconnect_popup();
            break;
        }
        
        char *up_buffer = convert_uppercase(buffer);
        manage_game(&game, up_buffer);

        // If the message is not "A1", make an AI move if applicable
        if (strcmp(buffer, "A1") != 0){
            if (ia == 1){
                ia_move(&game);
            }
        }
    }

    return NULL;
}

int setup_client() {
    struct sockaddr_in serv_addr;
    pthread_t recv_thread;
    pthread_t send_thread;

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_message("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port));

    // Convertir l'adresse IPv4 et IPv6 de texte en binaire
    if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
        log_message("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connexion au serveur
    int connection_status = -1;
    while (connection_status < 0) {
        connection_status = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (connection_status < 0) {
            printf("\nConnection Failed. Retrying in 5 seconds...\n");
            sleep(5);
        }
    }
    printf("\nConnexion réussie !\n");

    // Créer un thread pour envoyer les messages au serveur
    if (ia == 0){
        pthread_create(&send_thread, NULL, send_message_client, NULL);
    }
    // Créer un thread pour recevoir les messages du serveur
    if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0) {
        log_message("Failed to create thread\n");
        return -1;
    }

    return 0;
}