#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 33333

int sock = 0;

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

int main() {
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

    // Boucle pour envoyer des messages au serveur
    while (1) {
        printf("You: ");
        fgets(message, 1024, stdin);
        message[strcspn(message, "\n")] = '\0'; // Enlever le saut de ligne
        send(sock, message, strlen(message), 0);

        if (strcmp(message, "exit") == 0) {
            break;
        }
    }

    // Fermer le socket
    close(sock);

    return 0;
}