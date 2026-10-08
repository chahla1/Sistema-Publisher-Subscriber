#ifndef COMMUNICATION_H
#define COMMUNICATION_H

int connectToMaster(const char *ip, int port);
int sendMessage(int connection, const char *message);
int receiveMessage(int connection, char *buffer, int buffer_size);
void closeConnection(int connection);

#endif
