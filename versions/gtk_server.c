#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <asm-generic/socket.h>
#include <pthread.h>
#include <signal.h>

#define ROWS 7
#define COLS 10
#define PORT 33333

int server_fd;
int new_socket;

void handle_sigint(int sig) {
    close(server_fd);
    printf("Server closed\n");
    exit(0);
}

void on_button_clicked(GtkWidget *button, gpointer data) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button)); // Obtenir l'étiquette du bouton

    char message[1024];
    strncpy(message, label, sizeof(message) - 1); // Copier le label dans message
    message[sizeof(message) - 1] = '\0'; // Assurez-vous que le message est nul-terminé

    // Envoi d'un message de réponse au client
    send(new_socket, message, strlen(message), 0);
    gtk_widget_set_sensitive(button, FALSE);
}

void *client_handler(void *arg) {
    // Utiliser la variable globale new_socket
    int valread;
    char buffer[1024] = {0};
    printf("New socket !\n");

    while (1) {
        // Lecture du message reçu du client
        valread = read(new_socket, buffer, 1024);
        if (valread <= 0) {
            // Si la lecture échoue ou si le client se déconnecte, sortir de la boucle
            printf("Client disconnected or read error\n");
            break;
        }
        buffer[valread] = '\0'; // Assurez-vous que le buffer est une chaîne de caractères valide
        printf("Received: %s\n", buffer);

        // Comparer le contenu de la chaîne avec "exit"
        if (strcmp(buffer, "exit") == 0) {
            printf("Client requested to close the connection\n");
            break;
        }
    }

    // Fermer le socket du client
    close(new_socket);

    pthread_exit(NULL);
}

void *server_thread(void *arg) {
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Création d'un socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configuration du socket pour permettre la réutilisation de l'adresse
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Lier le socket à l'adresse et au port spécifiés
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Mettre le serveur en mode écoute
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Accepter la connexion entrante
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Créer un thread pour gérer le client
        pthread_t thread;
        pthread_create(&thread, NULL, client_handler, NULL);
    }
}

void setupUI(GtkWidget **window, GtkWidget **grid, GtkWidget *buttons[ROWS][COLS]) {
    // Création d'une nouvelle fenêtre
    *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(*window), "Chomp");
    gtk_window_set_default_size(GTK_WINDOW(*window), 600, 400);

    // Création d'une grille
    *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(*window), *grid);

    // Création des boutons pour les cases et ajout à la grille
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS - 1; j++) { // Ne pas créer de bouton pour la dernière colonne
            char label[3]; // Pour stocker le label du bouton
            snprintf(label, sizeof(label), "%c%d", 'a' + j, i + 1); // Créer des labels comme a1, b1, c1, etc.
            buttons[i][j] = gtk_button_new_with_label(label);
            gtk_grid_attach(GTK_GRID(*grid), buttons[i][j], j, i, 1, 1); // Positionner les boutons

            // Connecter le signal de clic du bouton à la fonction de rappel
            g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(on_button_clicked), NULL);
        }
    }

    // Connexion de l'événement de destruction de la fenêtre
    g_signal_connect(*window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Afficher la fenêtre
    gtk_widget_show_all(*window);
}

int main(int argc, char *argv[]) {
    // Gestion du signal SIGINT pour fermer le serveur proprement
    signal(SIGINT, handle_sigint);

    // Création d'un thread pour gérer les connexions des clients
    pthread_t server_thread_id;
    pthread_create(&server_thread_id, NULL, server_thread, NULL);

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *buttons[ROWS][COLS];

    // Initialisation de GTK
    gtk_init(&argc, &argv);

    // Configuration de l'interface utilisateur
    setupUI(&window, &grid, buttons);

    // Lancer la boucle d'événements GTK
    gtk_main();

    return 0;
}