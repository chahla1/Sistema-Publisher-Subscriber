#include "communication.h"
#include "socket_layer.h"

int connectToMaster(const char *ip, int port) {
    int socket_fd = socketCreate();

    if (socketConnect(socket_fd, ip, port) != 0) {
        socketClose(socket_fd);
        return -1;
    }

    return socket_fd;
}

int sendMessage(int connection, const char *message) {
    return socketSend(connection, message);
}

int receiveMessage(int connection, char *buffer, int buffer_size) {
    return socketReceive(connection, buffer, buffer_size);
}

void closeConnection(int connection) {
    socketClose(connection);
}
