#include "../include/utils.h"
#include "../include/game.h"
#include "../include/game_logic.h"
#include "../include/server.h"
#include "../include/ia.h"

int total_connexions = 0;

void handle_sigint(int sig) {
    close(server_fd);
    log_message("Server closed\n");
    exit(0);
}

void *send_message_server(void *arg) {
    char text[1024];

    while (1) {
        fgets(text, 1024, stdin);
        text[strcspn(text, "\n")] = '\0';
        char *up_text = convert_uppercase(text);
        if (current_player != player) {
            int status = manage_game(&game, up_text);
            if (status == 0){
                send_message(up_text);
            }
        }
    }
    return NULL;
}

void *client_handler(void *arg) {
    // Utiliser la variable globale client_socket
    int valread;
    char buffer[1024] = {0};
    printf("\nNouvelle connexion !\n");
    if (ia == 0){
        pthread_t send_thread;
        pthread_create(&send_thread, NULL, send_message_server, NULL);
    }
    while (1) {
        // Lecture du message reçu du client
        valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            // Si la lecture échoue ou si le client se déconnecte, sortir de la boucle
            total_connexions--;
            log_message("Client disconnected or read error\n");
            close(client_socket);
            break;
        }
        buffer[valread] = '\0'; // Assurez-vous que le buffer est une chaîne de caractères valide
        log_message("Received: %s\n", buffer);

        // Appeler manage_game avec le code de la grille reçu et mettre à jour le jeu
        char *up_buffer = convert_uppercase(buffer);
        manage_game(&game, up_buffer);

        if (strcmp(buffer, "A1") != 0){
            if (ia == 1){
                ia_move(&game);
            }
        }

        // Comparer le contenu de la chaîne avec "exit"
        if (strcmp(buffer, "exit") == 0) {
            log_message("Client requested to close the connection\n");
            close(client_socket);
            break;
        }
    }

    // Fermer le socket du client
    close(client_socket);

    pthread_exit(NULL);
}

void *server_thread(void *arg) {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Création d'un socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        log_message("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configuration du socket pour permettre la réutilisation de l'adresse
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        log_message("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(port));

    // Lier le socket à l'adresse et au port spécifiés
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        log_message("bind failed");
        exit(EXIT_FAILURE);
    }

    // Mettre le serveur en mode écoute
    if (listen(server_fd, 3) < 0) {
        log_message("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Accepter la connexion entrante
        int socket;
        if ((socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            log_message("accept failed");
            exit(EXIT_FAILURE);
        }
        
        // Gérer le nombre total de connexions
        total_connexions++;
        if (total_connexions > 1) {
            total_connexions--;
            send(socket, "exit", strlen("exit"), 0);
            continue;
        }
        client_socket = socket;

        // Créer un thread pour gérer le client
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, NULL);
    }
}
    
void setup_server() {
    signal(SIGINT, handle_sigint);
    // Création d'un thread pour gérer les connexions des clients
    pthread_t server_thread_id;
    pthread_create(&server_thread_id, NULL, server_thread, NULL);
}