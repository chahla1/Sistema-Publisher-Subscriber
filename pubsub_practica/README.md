# Práctica 1 - Sistema Pub/Sub

Implementación básica de un sistema Publisher/Subscriber en C usando procesos hijos y sockets TCP encapsulados mediante capas de abstracción.

## Estructura

- `main.c`: punto de entrada del Master.
- `master.c/.h`: Master/Broker y tabla de suscripciones.
- `pubsub_client.c`: proceso hijo P1, P2 o P3.
- `pubsub.c/.h`: abstracción Publisher/Subscriber.
- `communication.c/.h`: abstracción de comunicación.
- `socket_layer.c/.h`: única capa que utiliza directamente las funciones de sockets.
- `protocol.h`: constantes del protocolo.
- `Makefile`: compilación y ejecución.

## Arquitectura

Aplicación
→ Pub/Sub
→ Comunicación
→ Socket

La aplicación no utiliza directamente `socket()`, `connect()`, `send()`, `recv()` ni `close()`.


```text
SUB <topic>
UNSUB <topic>
PUB <topic> <tipo> <valor>
```

Tipos usados en el ejemplo:

- `INT`
- `FLOAT`
- `STRING` 

## Suscripciones de ejemplo

P1:
- temperatura
- estado

P2:
- alarma

P3:
- temperatura
- alarma

## Publicaciones de ejemplo

P2 publica:

PUB temperatura FLOAT 23.5


P3 publica:

PUB alarma INT 1
PUB estado INT 1



