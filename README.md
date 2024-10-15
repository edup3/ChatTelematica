## 1. Introduccion
## 2. Desarrollo
Para el desarrollo tuvimos en cuenta los siguientes requisitios especificados en el backlog del proyecto
* https://github.com/users/edup3/projects/6

### TCP o UDP  
LLegamos a la conclusion de usar TCP para este proyecto, ya que al ser una app de mensajeria o chat deicidimos priorizar la calidad y seguridad de los datos antes que la velocidad. Podemos ver en la funcion protocol_init donde se especifica el uso de TCP.  

    protocol->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
SOCK_STREAM define el uso de TCP, si quisieramos usar UDP tendriamos que usar SOCK_DGRAM
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
Del lado del servidor cada que se conecta un cliente se ejecuta una funcion handle_client() en un hilo nuevo que se encarga de manejar la conexion con cada cliente



## 3. Aspectos Logrados y No Logrados
## 4. Conclusiones
## 5. Referencias
* https://csperkins.org/teaching/2007-2008/networked-systems/lecture04.pdf
* https://beej.us/guide/bgc/html/split/
* https://beej.us/guide/bgnet/html/split/
