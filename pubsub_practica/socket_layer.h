#ifndef SOCKET_LAYER_H
#define SOCKET_LAYER_H

int socketCreate(void);
int socketConnect(int socket_fd, const char *ip, int port);
int socketSend(int socket_fd, const char *message);
int socketReceive(int socket_fd, char *buffer, int buffer_size);
void socketClose(int socket_fd);
int socketCreateServer(int port);
int socketAccept(int server_fd);

#endif
