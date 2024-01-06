#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

/*Función para conectar con el servidor y enviar el mensaje*/
int send_message_to_server(const char *message) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error al crear el socket del cliente"); /*manejo de errores*/
        return -1;
    }
    /*Estructura de almacenamiento de información de servidor*/
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /*Conexion al servidor mediante socket de cliente*/
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al conectar con el servidor");
        close(client_socket);
        return -1;
    }
    /*Envío de mensajes a través del socket cliente*/
    ssize_t bytes_sent = send(client_socket, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("Error al enviar datos al servidor");
        close(client_socket);
        return -1;
    }
    /*Cerrando el socket de cliente*/
    close(client_socket);
    return 0;
}

int main() {
    char message[BUFFER_SIZE];
    char command;
    int n1, n2, n3, n4, n5;

    /* Solicitar al usuario que ingrese el comando (A, B, C, D) */
    printf("Ingrese el comando (A, B, C, D): ");
    scanf(" %c", &command);

    /* Verificar el comando ingresado y solicitar los números */
    if (command == 'A' || command == 'B' || command == 'C') {
        printf("Ingrese cinco números separados por comas: ");
        scanf("%d,%d,%d,%d,%d", &n1, &n2, &n3, &n4, &n5);
    }

    /* Construir el mensaje con el formato correcto */
    sprintf(message, "<SYN>%c,%d,%d,%d,%d,%d<STX>", command, n1, n2, n3, n4, n5);

    printf("Mensaje enviado al servidor: %s\n", message);

    /* Función de envío de mensaje al servidor */
    if (send_message_to_server(message) == 0) {
        printf("Mensaje enviado correctamente.\n");
    } else {
        printf("Error al enviar el mensaje.\n");
        return -1;
    }

    return 0;
}
