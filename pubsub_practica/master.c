#include "master.h"
#include "communication.h"
#include "socket_layer.h"
#include "protocol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_CHILDREN 3
#define MAX_TOPICS 20
#define MAX_SUBSCRIBERS 10

typedef struct {
    char topic[MAX_TOPIC_SIZE];
    int clients[MAX_SUBSCRIBERS];
    int clientCount;
} TopicSubscription;

static TopicSubscription topics[MAX_TOPICS];
static int topicCount = 0;

static int findTopic(const char *topic) {
    for (int i = 0; i < topicCount; i++) {
        if (strcmp(topics[i].topic, topic) == 0) {
            return i;
        }
    }
    return -1;
}

static int createTopic(const char *topic) {
    if (topicCount >= MAX_TOPICS) {
        return -1;
    }

    strcpy(topics[topicCount].topic, topic);
    topics[topicCount].clientCount = 0;
    topicCount++;

    return topicCount - 1;
}

static void handleSubscribe(int client, const char *topic) {
    int index = findTopic(topic);

    if (index == -1) {
        index = createTopic(topic);
    }

    if (index == -1) {
        return;
    }

    TopicSubscription *subscription = &topics[index];

    for (int i = 0; i < subscription->clientCount; i++) {
        if (subscription->clients[i] == client) {
            return;
        }
    }

    if (subscription->clientCount < MAX_SUBSCRIBERS) {
        subscription->clients[subscription->clientCount++] = client;

        printf(
            "[MASTER] Cliente FD=%d suscrito a %s\n",
            client,
            topic
        );
        fflush(stdout);
    }
}

static void handleUnsubscribe(int client, const char *topic) {
    int index = findTopic(topic);

    if (index == -1) {
        return;
    }

    TopicSubscription *subscription = &topics[index];

    for (int i = 0; i < subscription->clientCount; i++) {
        if (subscription->clients[i] == client) {
            for (int j = i; j < subscription->clientCount - 1; j++) {
                subscription->clients[j] = subscription->clients[j + 1];
            }

            subscription->clientCount--;

            printf(
                "[MASTER] Cliente FD=%d eliminado de %s\n",
                client,
                topic
            );
            fflush(stdout);
            return;
        }
    }
}

static void handlePublish(
    const char *topic,
    const char *type,
    const char *value
) {
    int index = findTopic(topic);

    if (index == -1) {
        printf("[MASTER] Nadie suscrito a %s\n", topic);
        fflush(stdout);
        return;
    }

    char message[MAX_MESSAGE_SIZE];

    snprintf(
        message,
        sizeof(message),
        "MSG %s %s %s\n",
        topic,
        type,
        value
    );

    TopicSubscription *subscription = &topics[index];

    printf(
        "[MASTER] PUBLICACIÓN %s = %s -> %d suscriptores\n",
        topic,
        value,
        subscription->clientCount
    );
    fflush(stdout);

    for (int i = 0; i < subscription->clientCount; i++) {
        sendMessage(subscription->clients[i], message);
    }
}

static void processMessage(int client, char *message) {
    char command[16];
    char topic[MAX_TOPIC_SIZE];
    char type[32];
    char value[MAX_VALUE_SIZE];

    if (sscanf(message, "%15s", command) != 1) {
        return;
    }

    if (strcmp(command, "SUB") == 0) {
        if (sscanf(message, "%*s %63s", topic) == 1) {
            handleSubscribe(client, topic);
        }
    }
    else if (strcmp(command, "UNSUB") == 0) {
        if (sscanf(message, "%*s %63s", topic) == 1) {
            handleUnsubscribe(client, topic);
        }
    }
    else if (strcmp(command, "PUB") == 0) {
        if (sscanf(
                message,
                "%*s %63s %31s %127[^\n]",
                topic,
                type,
                value
            ) == 3) {
            handlePublish(topic, type, value);
        }
    }
}

static void launchChildren(void) {
    for (int i = 1; i <= MAX_CHILDREN; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            char processNumber[10];
            snprintf(processNumber, sizeof(processNumber), "%d", i);

            execl(
                "./pubsub_client",
                "./pubsub_client",
                processNumber,
                (char *)NULL
            );

            perror("execl");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        printf("[MASTER] Lanzado P%d PID=%d\n", i, (int)pid);
        fflush(stdout);
    }
}

void startMaster(int port) {
    int server = socketCreateServer(port);

    printf("\n=============================\n");
    printf("       MASTER / BROKER\n");
    printf("=============================\n");
    printf("Puerto: %d\n\n", port);
    fflush(stdout);

    launchChildren();

    int clients[MAX_CHILDREN];

    for (int i = 0; i < MAX_CHILDREN; i++) {
        clients[i] = socketAccept(server);

        if (clients[i] < 0) {
            fprintf(stderr, "[MASTER] Error aceptando cliente\n");
            continue;
        }

        printf(
            "[MASTER] Cliente conectado: FD=%d\n",
            clients[i]
        );
        fflush(stdout);
    }

    while (1) {
        for (int i = 0; i < MAX_CHILDREN; i++) {
            if (clients[i] < 0) {
                continue;
            }

            char message[MAX_MESSAGE_SIZE];

            int bytes = receiveMessage(
                clients[i],
                message,
                sizeof(message)
            );

            if (bytes > 0) {
                processMessage(clients[i], message);
            }
            else if (bytes == 0) {
                printf(
                    "[MASTER] Cliente FD=%d desconectado\n",
                    clients[i]
                );
                fflush(stdout);

                clients[i] = -1;
            }
        }
    }
}
