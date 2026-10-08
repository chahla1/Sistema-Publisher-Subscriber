#include "socket_layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int socketCreate(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return fd;
}

int socketConnect(int socket_fd, const char *ip, int port) {
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &server.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }

    if (connect(socket_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return -1;
    }

    return 0;
}

int socketSend(int socket_fd, const char *message) {
    return (int)send(socket_fd, message, strlen(message), 0);
}

int socketReceive(int socket_fd, char *buffer, int buffer_size) {
    int bytes = (int)recv(socket_fd, buffer, buffer_size - 1, 0);

    if (bytes > 0) {
        buffer[bytes] = '\0';
    }

    return bytes;
}

void socketClose(int socket_fd) {
    close(socket_fd);
}

int socketCreateServer(int port) {
    int server_fd = socketCreate();

    int option = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int socketAccept(int server_fd) {
    struct sockaddr_in client;
    socklen_t client_length = sizeof(client);

    int client_fd = accept(
        server_fd,
        (struct sockaddr *)&client,
        &client_length
    );

    if (client_fd < 0) {
        perror("accept");
        return -1;
    }

    return client_fd;
}
