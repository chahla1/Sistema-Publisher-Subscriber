#ifndef PUBSUB_H
#define PUBSUB_H

typedef void (*MessageCallback)(const char *topic, const char *value);

int pubsubInit(int connection);
int subscribe(const char *topic, MessageCallback callback);
int unsubscribe(const char *topic);
int publish(const char *topic, const char *type, const char *value);
void pubsubListen(void);
void pubsubClose(void);

#endif
