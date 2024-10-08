#include <stdio.h>
#include <string.h>
#include "protocol.h"

#define PORT 36195

void handle_client(MyChatProtocol* client_protocol) {
    char buffer[1024];
    while (1) {
        int bytes_received = protocol_receive(client_protocol, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Cliente desconectado.\n");
            break;
        }
        printf("Mensaje recibido: %s\n", buffer);
        
        // Enviar mensaje de vuelta al cliente
        protocol_send(client_protocol, "Echo: ");
        protocol_send(client_protocol, buffer);
    }
    protocol_close(client_protocol);
}

int main() {
    MyChatProtocol* server_protocol = protocol_init(PORT, NULL, 1);  // Inicializar servidor
    printf("Servidor escuchando en el puerto %d...\n", PORT);

    while (1) {
        MyChatProtocol* client_protocol = protocol_accept(server_protocol);
        if (client_protocol) {
            printf("Cliente conectado.\n");
            handle_client(client_protocol);
        }
    }

    protocol_close(server_protocol);  // Cerrar el servidor
    return 0;
}
