#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <netinet/in.h>

// Estructura para encapsular el socket y la dirección.
typedef struct {
    int socket_fd;
    struct sockaddr_in address;
} MyChatProtocol;

// Función para inicializar el protocolo en modo cliente o servidor.
MyChatProtocol* protocol_init(int port, const char* ip_address, int is_server);

// Función para aceptar conexiones de clientes (solo para el modo servidor).
MyChatProtocol* protocol_accept(MyChatProtocol* server_protocol);

// Funciones para enviar y recibir mensajes.
int protocol_send(MyChatProtocol* protocol, const char* message);
int protocol_receive(MyChatProtocol* protocol, char* buffer, int buffer_size);

// Cerrar el protocolo y liberar recursos.
void protocol_close(MyChatProtocol* protocol);

#endif
