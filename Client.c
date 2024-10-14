#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "protocol.h"

#define PORT 36195
#define SERVER_IP "3.88.25.149" // IP Publica del servidor

// Función para recibir mensajes del servidor en un hilo separado
void* receive_messages(void* arg) {
    MyChatProtocol* client_protocol = (MyChatProtocol*)arg;
    char buffer[1024];

    while (1) {
        int bytes_received = protocol_receive(client_protocol, buffer, sizeof(buffer) - 1);
        if (bytes_received <= 0) {
            printf("Servidor desconectado o error en la recepción.\n");
            break;
        }
        buffer[bytes_received] = '\0';  // Asegurarse de que el mensaje esté terminado en null
        printf("\n%s\n", buffer);
        printf("Tú: ");  // Mostrar el prompt de nuevo para que el usuario siga escribiendo
        fflush(stdout);  // Asegurarse de que el prompt se muestre inmediatamente
    }

    pthread_exit(NULL);
    return NULL;
}

int main() {
    MyChatProtocol* client_protocol = protocol_init(PORT, SERVER_IP, 0);  // Inicializar cliente

    // Crear un hilo para recibir mensajes del servidor
    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, (void*)client_protocol);

    char message[1024];
    while (1) {
        printf("Tú: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;  // Eliminar el salto de línea al final

        // Enviar el mensaje al servidor
        if (protocol_send(client_protocol, message) <= 0) {
            printf("Error al enviar el mensaje.\n");
            break;
        }
    }

    // Cerrar la conexión y terminar el programa
    protocol_close(client_protocol);
    pthread_cancel(receive_thread);  // Cancelar el hilo de recepción (si sigue activo)
    pthread_join(receive_thread, NULL);  // Esperar a que el hilo termine
    return 0;
}
