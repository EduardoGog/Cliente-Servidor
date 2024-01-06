#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

/*Con esta función se procesa mensaje de cliente y se genera una respuesta*/
void handle_message(char *message, char *response) {
    char cmd;
    int n1, n2, n3, n4, n5;
    int i, j;
    int sorted_numbers[5];
    if (sscanf(message, "<SYN>%c,%d,%d,%d,%d,%d<STX>", &cmd, &n1, &n2, &n3, &n4, &n5) == 6) {
        int sorted_numbers[5] = {n1, n2, n3, n4, n5}; 

        switch (cmd) {
            case 'A':
                /*Ordenar ascendentemente*/
                for (i = 0; i < 5 - 1; i++) {
                    for (j = 0; j < 5 - i - 1; j++) {
                        if (sorted_numbers[j] > sorted_numbers[j + 1]) {
                            int temp = sorted_numbers[j];
                            sorted_numbers[j] = sorted_numbers[j + 1];
                            sorted_numbers[j + 1] = temp;
                        }
                    }
                }       
                sprintf(response, "%d,%d,%d,%d,%d", sorted_numbers[0], sorted_numbers[1], sorted_numbers[2], sorted_numbers[3], sorted_numbers[4]);
                break;
            case 'B':
                /*Ordenar descendentemente*/
                for (i = 0; i < 5 - 1; i++) {
                    for (j = 0; j < 5 - i - 1; j++) {
                        if (sorted_numbers[j] < sorted_numbers[j + 1]) {
                            int temp = sorted_numbers[j];
                            sorted_numbers[j] = sorted_numbers[j + 1];
                            sorted_numbers[j + 1] = temp;
                        }
                    }
                }             
                sprintf(response, "%d,%d,%d,%d,%d", sorted_numbers[0], sorted_numbers[1], sorted_numbers[2], sorted_numbers[3], sorted_numbers[4]);
                break;
            case 'C':
                sprintf(response, "%d", n1 + n2 + n3 + n4 + n5);
                break;
            case 'D':
                sleep(3);
                strcpy(response, "Retardo de mensaje");
                return;
            default:
                strcpy(response, "Comando no encontrado");
                return;
        }
    } else if (sscanf(message, "<SYN>%c<STX>", &cmd) == 1) {
/*Manejo de errores en caso de que no se ingresen números y solo el comando*/
        switch (cmd) {
            case 'D':
                sleep(3);
                strcpy(response, "Retardo de mensaje");
                return;
            default:
                strcpy(response, "Comando no encontrado");
                return;
        }
    } else {
        strcpy(response, "Formato de mensaje incorrecto");
        return;
    }
}

int main() {
/*Variables, estructuras para lamecenar información de dirección cliente y servidor*/
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

	/*Creación de socket para servidor*/
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket");
        return -1;
    }

    /*Configurar la dirección del servidor*/
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    /*Vincular el socket a la dirección y al puerto por medio de "bind"*/
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error al vincular el socket");
        return -1;
    }

    /*Escuchar por conexiones entrantes*/
	/*función listen para poner al socket del servidor en modo de escucha para aceptar conexiones entrantes*/
    if (listen(server_socket, 1) == -1) {
        perror("Error al escuchar por conexiones");
        return -1;
    }

    printf("Servidor esperando conexiones en el puerto %d...\n", PORT);

    /*Aceptar conexiones entrantes*/
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("Error al aceptar la conexión");
        return -1;
    }

    printf("Cliente conectado\n");

    /*Recibir datos del cliente*/
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Error al recibir datos");
        return -1;
    }
	/*Terminador nulo*/
    buffer[bytes_received] = '\0'; 

    /*Procesar el mensaje y generar una respuesta*/
    char response[BUFFER_SIZE];
    handle_message(buffer, response);

    /*Enviar la respuesta al cliente*/
    ssize_t bytes_sent = send(client_socket, response, strlen(response), 0);
    if (bytes_sent == -1) {
        perror("Error al enviar datos al cliente");
        return -1;
    }

    printf("Respuesta enviada al cliente: %s\n", response);

    /*Cerrar los sockets*/
    close(client_socket);
    close(server_socket);

    return 0;
}
