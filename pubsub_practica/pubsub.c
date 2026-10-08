#include "pubsub.h"
#include "communication.h"
#include "protocol.h"

#include <stdio.h>
#include <string.h>

#define MAX_SUBSCRIPTIONS 20

typedef struct {
    char topic[MAX_TOPIC_SIZE];
    MessageCallback callback;
} Subscription;

static int connection = -1;
static Subscription subscriptions[MAX_SUBSCRIPTIONS];
static int subscriptionCount = 0;

int pubsubInit(int connection_fd) {
    connection = connection_fd;
    subscriptionCount = 0;
    return 0;
}

int subscribe(const char *topic, MessageCallback callback) {
    if (subscriptionCount >= MAX_SUBSCRIPTIONS) {
        return -1;
    }

    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "SUB %s\n", topic);

    if (sendMessage(connection, message) < 0) {
        return -1;
    }

    strcpy(subscriptions[subscriptionCount].topic, topic);
    subscriptions[subscriptionCount].callback = callback;
    subscriptionCount++;

    return 0;
}

int unsubscribe(const char *topic) {
    char message[MAX_MESSAGE_SIZE];
    snprintf(message, sizeof(message), "UNSUB %s\n", topic);

    if (sendMessage(connection, message) < 0) {
        return -1;
    }

    for (int i = 0; i < subscriptionCount; i++) {
        if (strcmp(subscriptions[i].topic, topic) == 0) {
            for (int j = i; j < subscriptionCount - 1; j++) {
                subscriptions[j] = subscriptions[j + 1];
            }
            subscriptionCount--;
            break;
        }
    }

    return 0;
}

int publish(const char *topic, const char *type, const char *value) {
    char message[MAX_MESSAGE_SIZE];

    snprintf(
        message,
        sizeof(message),
        "PUB %s %s %s\n",
        topic,
        type,
        value
    );

    return sendMessage(connection, message);
}

void pubsubListen(void) {
    char message[MAX_MESSAGE_SIZE];

    while (1) {
        int bytes = receiveMessage(connection, message, sizeof(message));

        if (bytes <= 0) {
            break;
        }

        if (strncmp(message, "MSG ", 4) == 0) {
            char topic[MAX_TOPIC_SIZE];
            char type[32];
            char value[MAX_VALUE_SIZE];

            if (sscanf(
                    message + 4,
                    "%63s %31s %127[^\n]",
                    topic,
                    type,
                    value
                ) == 3) {

                for (int i = 0; i < subscriptionCount; i++) {
                    if (strcmp(subscriptions[i].topic, topic) == 0) {
                        subscriptions[i].callback(topic, value);
                    }
                }
            }
        }
    }
}

void pubsubClose(void) {
    if (connection != -1) {
        closeConnection(connection);
        connection = -1;
    }
}
