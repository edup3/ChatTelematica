#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "protocol.h"

#define MAX_CLIENTS 10  // Número máximo de clientes que el servidor puede manejar
#define PORT 36195

// Estructura para almacenar la información de cada cliente
typedef struct {
    MyChatProtocol* protocol;  // Protocolo para la comunicación con el cliente
    int id;                    // Identificador único del cliente
    char username[50];         // Nombre de usuario del cliente
    int connected_to;          // ID del cliente con el que está conectado (0 si no está en un chat)
} Client;

Client* clients[MAX_CLIENTS];  // Arreglo para almacenar los clientes conectados
int client_count = 0;          // Número de clientes actualmente conectados
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;  // Mutex para proteger el acceso a la lista de clientes

// Función para agregar un cliente a la lista de manera segura
void add_client(MyChatProtocol* client_protocol, int client_id, const char* username) {
    pthread_mutex_lock(&clients_mutex);
    if (client_count < MAX_CLIENTS) {
        clients[client_count] = (Client*)malloc(sizeof(Client));
        clients[client_count]->protocol = client_protocol;
        clients[client_count]->id = client_id;
        clients[client_count]->connected_to = 0;  // No está conectado a otro cliente inicialmente
        strncpy(clients[client_count]->username, username, sizeof(clients[client_count]->username) - 1);
        clients[client_count]->username[sizeof(clients[client_count]->username) - 1] = '\0';
        client_count++;
        printf("Cliente %d (%s) conectado.\n", client_id, username);
    } else {
        printf("Número máximo de clientes alcanzado.\n");
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Función para enviar la lista de clientes disponibles para chatear
void send_client_list(MyChatProtocol* client_protocol, int client_id) {
    char list_message[1024] = "Usuarios disponibles para chat:\n";
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i]->id != client_id && clients[i]->connected_to == 0) {
            char client_info[100];
            snprintf(client_info, sizeof(client_info), "%d: %s\n", clients[i]->id, clients[i]->username);
            strncat(list_message, client_info, sizeof(list_message) - strlen(list_message) - 1);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    protocol_send(client_protocol, list_message);
}

// Función para encontrar un cliente por su ID
Client* find_client_by_id(int client_id) {
    pthread_mutex_lock(&clients_mutex);
    Client* found_client = NULL;
    for (int i = 0; i < client_count; i++) {
        if (clients[i]->id == client_id) {
            found_client = clients[i];
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
    return found_client;
}

// Función para manejar los mensajes de un cliente
void* handle_client(void* arg) {
    Client* client = (Client*)arg;
    MyChatProtocol* client_protocol = client->protocol;
    int client_id = client->id;
    char buffer[1024];

    // Enviar la lista de usuarios disponibles para chat
    send_client_list(client_protocol, client_id);

    // Esperar la selección del cliente con el que desea chatear
    protocol_send(client_protocol, "Escribe el ID del usuario con el que deseas chatear:");
    int selected_id;
    int bytes_received = protocol_receive(client_protocol, buffer, sizeof(buffer) - 1);
    if (bytes_received <= 0) {
        printf("Cliente %d (%s) desconectado antes de elegir un usuario.\n", client_id, client->username);
        free(client);
        pthread_exit(NULL);
    }
    buffer[bytes_received] = '\0';
    sscanf(buffer, "%d", &selected_id);

    // Buscar al cliente seleccionado y establecer la conexión
    Client* selected_client = find_client_by_id(selected_id);
    if (selected_client && selected_client->connected_to == 0) {
        pthread_mutex_lock(&clients_mutex);
        client->connected_to = selected_id;
        selected_client->connected_to = client_id;
        pthread_mutex_unlock(&clients_mutex);
        char notification[200];
        snprintf(notification, sizeof(notification), "El usuario %d (%s) desea conectarse contigo. Escribe su ID para aceptar la conexión:", client_id, client->username);
        protocol_send(selected_client->protocol, notification);

        // Intercambio de mensajes entre los dos clientes conectados
        while (1) {
            bytes_received = protocol_receive(client_protocol, buffer, sizeof(buffer) - 1);
            if (bytes_received <= 0) {
                printf("Cliente %d (%s) desconectado.\n", client_id, client->username);
                break;
            }
            buffer[bytes_received] = '\0';

            // Enviar el mensaje al cliente conectado
            char outgoing_message[1024];
            snprintf(outgoing_message, sizeof(outgoing_message), "%s dice: %.900s", client->username, buffer);
            protocol_send(selected_client->protocol, outgoing_message);
        }
    } else {
        protocol_send(client_protocol, "Usuario no disponible para chatear.");
    }

    // Limpiar la conexión entre los dos clientes si alguno se desconecta
    pthread_mutex_lock(&clients_mutex);
    if (selected_client && selected_client->connected_to == client_id) {
        selected_client->connected_to = 0;
        protocol_send(selected_client->protocol, "El otro usuario se ha desconectado.");
    }
    client->connected_to = 0;
    pthread_mutex_unlock(&clients_mutex);

    // Cerrar la conexión y liberar recursos
    protocol_close(client_protocol);
    free(client);
    pthread_exit(NULL);
    return NULL;
}

// Función principal del servidor
int main() {
    MyChatProtocol* server_protocol = protocol_init(PORT, NULL, 1);  // Inicializar el servidor
    printf("Servidor escuchando en el puerto %d...\n", PORT);
    int next_client_id = 1;

    while (1) {
        MyChatProtocol* client_protocol = protocol_accept(server_protocol);
        if (client_protocol) {
            // Asignar un nombre de usuario predeterminado "Cliente_X" al conectar
            char default_username[50];
            snprintf(default_username, sizeof(default_username), "Cliente_%d", next_client_id);

            Client* new_client = (Client*)malloc(sizeof(Client));
            new_client->protocol = client_protocol;
            new_client->id = next_client_id;
            strncpy(new_client->username, default_username, sizeof(new_client->username) - 1);
            new_client->username[sizeof(new_client->username) - 1] = '\0';
            new_client->connected_to = 0;

            add_client(client_protocol, next_client_id, default_username);

            pthread_t thread_id;
            pthread_create(&thread_id, NULL, handle_client, (void*)new_client);
            pthread_detach(thread_id);

            next_client_id++;
        }
    }

    protocol_close(server_protocol);  // Cerrar el servidor
    return 0;
}
