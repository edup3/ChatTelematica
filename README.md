## 1. Introduccion
## 2. Desarrollo
Para el desarrollo tuvimos en cuenta los siguientes requisitios especificados en el backlog del proyecto
* https://github.com/users/edup3/projects/6

### TCP o UDP  
Decidimos usar TCP en nuestra aplicación de chat porque ofrece una comunicación fiable y orientada a la conexión, lo cual es clave cuando se trata de transmitir mensajes de manera precisa y segura. TCP garantiza que los datos lleguen en orden y sin pérdidas, utilizando mecanismos como la retransmisión de paquetes en caso de error y el control de flujo para evitar la saturación de la red. Esto asegura que los mensajes enviados en el chat se reciban completos, sin errores ni desorden, manteniendo la integridad de la comunicación.

Podemos ver cómo esto se refleja en el código cuando en la función protocol_init se especifica el uso de TCP de la siguiente manera:

    protocol->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
SOCK_STREAM define el uso de TCP, si quisieramos usar UDP tendriamos que usar SOCK_DGRAM

Por otro lado, UDP, aunque es más rápido al no requerir la misma gestión de conexión que TCP, no garantiza la entrega ni el orden de los mensajes. En un chat, esto podría resultar en mensajes perdidos o desordenados, lo cual sería una experiencia negativa para los usuarios, ya que cada mensaje es importante para mantener una conversación coherente. Por eso, aunque TCP puede ser un poco más lento que UDP, la prioridad en este caso es garantizar una comunicación fiable, haciendo que TCP sea la mejor opción.
____
### Diagramas  
Tambien nos guiamos de los siguientes diagramas para hacernos una idea de la estructura del proyecto y su flujo  

### Diagrama de Componentes
![Telematica Chat](https://github.com/user-attachments/assets/5033be72-31db-45c1-9bf0-521882f9e373)  
Decidimos usar este diagrama en vez de el diagrama de clases
ya que al proyecto estar desarrollado en C que no es un lenguaje orientado a objetos, decidimos que podiamos usar el de componentes para ayudarnos 
a comprender los archivos del proyecto y como se relacionan entre ellos. Tomamos la decision de usar un archivo Protocolo.c para abstraer toda la logica de los sockets en 
un solo archivo y asi seprarar las responsabilidades de cada archivo, usando el archivo protocolo.h como interfaz para el cliente y el servidor.
### Diagrama de flujo para el servidor
![image](https://github.com/user-attachments/assets/531faeb4-f63c-4b69-98aa-3f419dcd93ad)

### Diagrama de flujo para el cliente
![image](https://github.com/user-attachments/assets/af0ec4df-f9ab-43b1-bba4-3340959ab307)

A traves de todos los archivos definimos varias estructuras que nos ayudarian para manejar la informacion de los clientes y de los sockets  
  
        typedef struct {
        int socket_fd; // Guarda el socket
        struct sockaddr_in address; //Guarda informacion relevante para el funcionamiento del socket(IP,Puerto)
    } MyChatProtocol; 

<br />
    
        typedef struct {
        MyChatProtocol* protocol;  // Protocolo para la comunicación con el cliente
        int id;                    // Identificador único del cliente
        char username[50];         // Nombre de usuario del cliente
        int connected_to;          // ID del cliente con el que está conectado (0 si no está en un chat)
    } Client;
Tambien utilizamos programacion de hilos para manejar la concurrencia tanto del lado del servidor, como del lado del cliente. Para asegurarnos de evitar errores al tener varios hilos accediendo a las mismas variables usamos mutex.
Del lado del servidor cada que se conecta un cliente se ejecuta una funcion handle_client() en un hilo nuevo que se encarga de manejar la conexion con cada cliente.

    pthread_create(&thread_id, NULL, handle_client, (void*)new_client);

    
Y del lado del cliente utilizamos hilos para que el cliente pueda mandar mensaje a su vez que los recibe.

    pthread_create(&receive_thread, NULL, receive_messages, (void*)client_protocol);



## 3. Aspectos Logrados y No Logrados

En este proyecto de chat en C utilizando sockets y TCP, se alcanzaron varios objetivos clave. El servidor y los clientes pueden conectarse y transmitir mensajes de manera confiable a través del protocolo TCP, priorizando la integridad y seguridad de los datos sobre la velocidad. Se implementó exitosamente la retransmisión de mensajes a múltiples clientes conectados, asegurando una comunicación fluida y concurrente. Además, el manejo básico de las conexiones entre el servidor y los clientes ha sido completado de manera satisfactoria, lo que permite una gestión estable del tráfico de mensajes.

Sin embargo, quedaron algunos aspectos sin implementar o mejorar, como la finalización correcta de la conexión del servidor y un manejo más robusto de los errores de conexión, que no fueron abordados. Aunque el chat cumple con las funcionalidades básicas, estos puntos representan áreas que podrían mejorar la estabilidad y la gestión de eventos excepcionales en el sistema.

## 4. Conclusiones
Este Proyecto nos permitio explorar a fondo la utilizacion de sockets y multithreading en C, asi como sus metodos y manejo de errores. Para el desarrollo de este proyecto tambien fue fundamental una buena planeacion antes de empezar a desarrollar,
lo cual nos permitio hacernos una idea de la estructura y funcionamiento de la aplicacion.
## 5. Referencias
* https://csperkins.org/teaching/2007-2008/networked-systems/lecture04.pdf
* https://beej.us/guide/bgc/html/split/
* https://beej.us/guide/bgnet/html/split/
