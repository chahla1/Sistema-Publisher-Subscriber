CC = gcc
CFLAGS = -Wall -Wextra -std=c11

COMMON = socket_layer.c communication.c pubsub.c

.PHONY: all clean run

all: master pubsub_client

master: main.c master.c master.h $(COMMON) socket_layer.h communication.h pubsub.h protocol.h
	$(CC) $(CFLAGS) -o master main.c master.c $(COMMON)

pubsub_client: pubsub_client.c $(COMMON) socket_layer.h communication.h pubsub.h protocol.h
	$(CC) $(CFLAGS) -o pubsub_client pubsub_client.c $(COMMON)

run: all
	./master

clean:
	rm -f master pubsub_client
