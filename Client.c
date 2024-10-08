#include <stdio.h>
#include <string.h>
#include "protocol.h"

#define PORT 36195
#define SERVER_IP "52.203.146.98"

int main() {
    MyChatProtocol* client_protocol = protocol_init(PORT, SERVER_IP, 0);  // Inicializar cliente
    char message[1024];
    char response[1024];

    while (1) {
        printf("Tú: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;  // Eliminar el salto de línea

        // Enviar mensaje al servidor
        protocol_send(client_protocol, message);

        // Recibir respuesta del servidor
        int bytes_received = protocol_receive(client_protocol, response, sizeof(response));
        if (bytes_received > 0) {
            printf("Servidor: %s\n", response);
        } else {
            printf("Desconectado del servidor.\n");
            break;
        }
    }

    protocol_close(client_protocol);  // Cerrar la conexión con el servidor
    return 0;
}
