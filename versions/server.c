#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <asm-generic/socket.h>

#define PORT 33333

int server_fd;

void handle_sigint(int sig) {
    close(server_fd);
    printf("Server closed\n");
    exit(0);
}

int main() {
    int new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Gestion du signal pour fermer le serveur proprement
    signal(SIGINT, handle_sigint);

    // Création du socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attachement du socket au port 33333
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Attachement du socket au port et écoute des connexions entrantes
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accepter les connexions entrantes
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Lecture du message reçu du client
        valread = read(new_socket, buffer, 1024);
        printf("Received: %s\n", buffer);

        // Envoi d'un message de réponse au client
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        // Fermer le socket du client
        close(new_socket);
    }

    return 0;
}
