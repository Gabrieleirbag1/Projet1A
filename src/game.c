#include "../include/utils.h"
#include "../include/game.h"
#include "../include/game_gtk.h"
#include "../include/game_logic.h"
#include "../include/server.h"
#include "../include/client.h"
#include "../include/ia.h"

//+++++ Déclaration des variables globales +++++//
char* port = "33333";
char* address = "127.0.0.1";

int local = 0;
int server = 0;
int client = 0;
int ia = 0;
int history_of_moves[100][2];  // Historique des mouvements (on suppose qu'il y aura un maximum de 100 mouvements). 
int history_index = 0;

int sock = 0;
int server_fd;
int client_socket;

char message[1024];
int player;
int current_player = 1;

GtkWidget *turn;
Game game;
FILE *log_file_server;
FILE *log_file_client;
FILE *log_file_local;

void init_log_files() {
    if (server == 1){
        log_file_server = fopen("game_log_server.log", "a");
        if (log_file_server == NULL) {
            perror("Failed to open server log file");
            exit(1);
        }
    } else if (client == 1){
        log_file_client = fopen("game_log_client.log", "a");
        if (log_file_client == NULL) {
            perror("Failed to open client log file");
            exit(1);
        }
    } else if (local == 1){
        log_file_local = fopen("game_log_local.log", "a");
        if (log_file_local == NULL) {
            perror("Failed to open local log file");
            exit(1);
        }
    }
    // Redirect stderr to the log file
    if (server == 1){
        dup2(fileno(log_file_server), STDERR_FILENO);
    } else if (client == 1){
        dup2(fileno(log_file_client), STDERR_FILENO);
    } else if (local == 1){
        dup2(fileno(log_file_local), STDERR_FILENO);
    }
}

void log_message(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Get the current date and time
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);

    // Print the timestamp and the log message
    FILE *log_file;
    if (server == 1){
        log_file = log_file_server;
    } else if (client == 1){
        log_file = log_file_client;
    } else if (local == 1){
        log_file = log_file_local;
    }    
    fprintf(log_file, "[%s] ", timestamp);
    vfprintf(log_file, format, args);

    va_end(args);
    fflush(log_file);
}

int process_arguments(int argc, char *argv[]) {
    char *ip = NULL;
    // Parcourir les arguments supplémentaires
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            local = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            server = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                port = argv[++i];
            }
        } else if (strcmp(argv[i], "-c") == 0) {
            client = 1;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                ip = argv[++i];
                // Split the address into IP and port
                address = strtok(ip, ":");
                port = strtok(NULL, ":");
            }
        } else if (strcmp(argv[i], "-ia") == 0) {
            ia = 1;
            printf("Mode IA activé\n");
        } else if (argv[i][0] == '-') {
            printf("Option inconnue : %s\n", argv[i]);
            printf("Usage: %s [-l] [-s <port>] [-c <address:port>] [-ia]\n", argv[0]);
            return 1;
        }
    }

    // Exécution des commandes en fonction des options définies
    if (local) {
        printf("Exécution en local\n");
        // Ajoutez ici la logique pour l'exécution en local
    } else if (server) {
        printf("Exécution en mode serveur sur le port %s\n", port ? port : "default");
        player = 1;
        if (ia) {
            // Ajoutez ici la logique pour le serveur avec IA
        } else {
            // Ajoutez ici la logique pour le serveur sans IA
        }
    } else if (client) {
        printf("Exécution en mode client vers l'adresse %s sur le port %s\n", ip ? ip : "default", port ? port : "default");
        player = 2;
        if (ia) {
            // Ajoutez ici la logique pour le client avec IA
        } else {
            // Ajoutez ici la logique pour le client sans IA
        }
    } else {
        printf("Aucune option valide fournie\n");
        printf("Usage: %s [-l] [-s <port>] [-c <address:port>] [-ia]\n", argv[0]);
        return 1;
    }
    return 0;
}

// Function to set up the network based on the mode (server or client)
void setup_network(){
    if (server == 1) {
        // Set up the server
        setup_server();
    } else if (client == 1) {
        // Set up the client
        setup_client();
    }
}

// Function to close all open resources (log files and sockets)
void close_all() {
    if (server == 1) {
        // Close the server log file and the server socket
        fclose(log_file_server);
        close(server_fd);
    } else if (client == 1) {
        // Close the client log file and the client socket
        fclose(log_file_client);
        close(sock);
    } else if (local == 1) {
        // Close the local log file
        fclose(log_file_local);
    }
}

int main(int argc, char *argv[]) {
    // Traitement des arguments
    if (process_arguments(argc, argv) == 0){
        // Initialiser les fichiers de log
        init_log_files();
        // Mettre en place le réseau
        setup_network();
        // Configuration de l'interface utilisateur
        setup_UI(argc, argv);
        // Fermer les logs et les sockets
        close_all();
    }

    return 0;
}