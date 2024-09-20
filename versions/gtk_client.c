#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define ROWS 7
#define COLS 10
#define PORT 33333

int sock = 0;
char message[1024];

void on_button_clicked(GtkWidget *button, gpointer data) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button)); // Obtenir l'étiquette du bouton

    char message[1024];
    strncpy(message, label, sizeof(message) - 1); // Copier le label dans message
    message[sizeof(message) - 1] = '\0'; // Assurez-vous que le message est nul-terminé

    send(sock, message, strlen(message), 0);
    gtk_widget_set_sensitive(button, FALSE);
}

void *receive_messages(void *arg) {
    char buffer[1024];
    int valread;

    while (1) {
        valread = read(sock, buffer, 1024);
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("Server: %s\n", buffer);
        }
    }

    return NULL;
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
    struct sockaddr_in serv_addr;
    char message[1024];
    pthread_t recv_thread;

    // Création du socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convertir l'adresse IPv4 et IPv6 de texte en binaire
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Créer un thread pour recevoir les messages du serveur
    if (pthread_create(&recv_thread, NULL, receive_messages, NULL) != 0) {
        printf("Failed to create thread\n");
        return -1;
    }

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *buttons[ROWS][COLS];

    // Initialisation de GTK
    gtk_init(&argc, &argv);

    // Configuration de l'interface utilisateur
    setupUI(&window, &grid, buttons);

    // Lancer la boucle d'événements GTK
    gtk_main();

    // Fermer le socket
    close(sock);

    return 0;
}