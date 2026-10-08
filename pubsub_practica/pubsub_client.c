#include "pubsub.h"
#include "communication.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void onMessage(const char *topic, const char *value) {
    printf(
        "[CALLBACK] Recibido %s = %s\n",
        topic,
        value
    );
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <numero_proceso>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int processNumber = atoi(argv[1]);

    printf("[P%d] Iniciando...\n", processNumber);
    fflush(stdout);

    sleep(1);

    int connection = connectToMaster("127.0.0.1", 5000);

    if (connection < 0) {
        fprintf(stderr, "[P%d] Error conectando al Master\n", processNumber);
        return EXIT_FAILURE;
    }

    printf("[P%d] Conectado al Master\n", processNumber);
    fflush(stdout);

    pubsubInit(connection);

    if (processNumber == 1) {
        subscribe("temperatura", onMessage);
        subscribe("estado", onMessage);

        printf("[P1] Suscrito a temperatura y estado\n");
        fflush(stdout);
    }
    else if (processNumber == 2) {
        subscribe("alarma", onMessage);

        printf("[P2] Suscrito a alarma\n");
        fflush(stdout);

        sleep(2);

        publish("temperatura", "FLOAT", "23.5");

        printf("[P2] Publicado temperatura = 23.5\n");
        fflush(stdout);
    }
    else if (processNumber == 3) {
        subscribe("temperatura", onMessage);
        subscribe("alarma", onMessage);

        printf("[P3] Suscrito a temperatura y alarma\n");
        fflush(stdout);

        sleep(3);

        publish("alarma", "INT", "1");

        printf("[P3] Publicado alarma = 1\n");
        fflush(stdout);

        publish("estado", "INT", "1");

        printf("[P3] Publicado estado = 1\n");
        fflush(stdout);
    }

    pubsubListen();
    pubsubClose();

    return EXIT_SUCCESS;
}
