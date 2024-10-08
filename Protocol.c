#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BACKLOG 5  // Número máximo de conexiones en cola (para el servidor)

// Función para inicializar el protocolo (cliente o servidor)
MyChatProtocol* protocol_init(int port, const char* ip_address, int is_server) {
    MyChatProtocol* protocol = (MyChatProtocol*)malloc(sizeof(MyChatProtocol));
    if (!protocol) {
        perror("Error al asignar memoria al protocolo");
        exit(EXIT_FAILURE);
    }

    // Crear socket TCP
    protocol->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (protocol->socket_fd == -1) {
        perror("Error al crear socket");
        free(protocol);
        exit(EXIT_FAILURE);
    }

    protocol->address.sin_family = AF_INET;
    protocol->address.sin_port = htons(port);
    
    if (is_server) {
        protocol->address.sin_addr.s_addr = INADDR_ANY;  // Servidor escucha en cualquier IP local
        // Enlazar el socket al puerto y dirección IP
        if (bind(protocol->socket_fd, (struct sockaddr*)&protocol->address, sizeof(protocol->address)) == -1) {
            perror("Error en bind");
            close(protocol->socket_fd);
            free(protocol);
            exit(EXIT_FAILURE);
        }
        // Escuchar conexiones
        if (listen(protocol->socket_fd, BACKLOG) == -1) {
            perror("Error en listen");
            close(protocol->socket_fd);
            free(protocol);
            exit(EXIT_FAILURE);
        }
    } else {
        // Cliente: convertir la IP a formato binario y conectarse
        if (inet_pton(AF_INET, ip_address, &protocol->address.sin_addr) <= 0) {
            perror("Error en inet_pton");
            close(protocol->socket_fd);
            free(protocol);
            exit(EXIT_FAILURE);
        }
        // Conectar al servidor
        if (connect(protocol->socket_fd, (struct sockaddr*)&protocol->address, sizeof(protocol->address)) == -1) {
            perror("Error en connect");
            close(protocol->socket_fd);
            free(protocol);
            exit(EXIT_FAILURE);
        }
    }

    return protocol;
}

// Función para aceptar conexiones del servidor (solo para modo servidor)
MyChatProtocol* protocol_accept(MyChatProtocol* server_protocol) {
    MyChatProtocol* client_protocol = (MyChatProtocol*)malloc(sizeof(MyChatProtocol));
    if (!client_protocol) {
        perror("Error al asignar memoria al cliente");
        exit(EXIT_FAILURE);
    }

    socklen_t addr_len = sizeof(client_protocol->address);
    client_protocol->socket_fd = accept(server_protocol->socket_fd, (struct sockaddr*)&client_protocol->address, &addr_len);
    if (client_protocol->socket_fd == -1) {
        perror("Error en accept");
        free(client_protocol);
        return NULL;
    }

    return client_protocol;
}

// Función para enviar datos a través del socket
int protocol_send(MyChatProtocol* protocol, const char* message) {
    return send(protocol->socket_fd, message, strlen(message), 0);
}

// Función para recibir datos a través del socket
int protocol_receive(MyChatProtocol* protocol, char* buffer, int buffer_size) {
    int bytes_received = recv(protocol->socket_fd, buffer, buffer_size, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';  // Añadir terminador nulo
    }
    return bytes_received;
}

// Función para cerrar el protocolo y liberar recursos
void protocol_close(MyChatProtocol* protocol) {
    close(protocol->socket_fd);
    free(protocol);
}
