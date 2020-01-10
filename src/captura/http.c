/**
 * @file			http.c
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 *
 */

/**
 * @file				http.c
 *						Copyright
 *						-----------------------------------------------
 * Copyright 2019 Alberto <1457159@campus.euss.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */
 /**
 *
 * @file				http.c
 *
 *						Descripción programa
 *						-----------------------------------------------
 * El programa realiza una consulta mediante el procolo HTTP. Su utilidad es obtener los segundos (cada cuanto tiene el sensor tiene que hacer una captura) y minutos (Mostrar si ha habido una alarma).
 */

 /**
 * @file				http.c
 *
 *						Librerias a incluir en el programa
 *						-----------------------------------------------
 \verbatim
		#include <stdio.h>
		#include <stdlib.h>
		#include <sys/types.h>
		#include <sys/socket.h>
		#include <string.h>
		#include <sys/socket.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <unistd.h>
		#include <string.h>
		#include "http.h""
 \endverbatim
 */
 /**
 * @file				http.c
 *
 *						Variables predefinidas en el programa
 *						-----------------------------------------------
 \verbatim
		\def #define REQUEST_MSG_SIZE 1024
		\def #define REPLY_MSG_SIZE  512
		\def #define SERVER_PORT_NUM 80
  \endverbatim
 */

 /**
 ** @file				http.c
 *
 *						Variables a utilizar en el programa
 *						-----------------------------------------------
 \verbatim
	@struct struct sockaddr_in serverAddr
	@note sockaddr_in:
		@brief Dentro de la esctructura sockaddr_in se tiene 3 campos importantes:
				@note sin_family: dominio (AF_INET).
				@note sin_port: puerto.
				@note sin addr: direccion del host.
	int	sockAddrSize;
	int	sFd;
	int	result;
	char	buffer[256];
	int	j=0;
	int	i = 0;
	int content = 0;
	char data_information[20];
	char *time_lectura;
	char *time_alarma;
	char delimitador[2] = ",";
 \endverbatim
 */

/**
 *
 * \fn int web_http(char ServerName[25], char* Archivo, int *minutos, int *segundos);
 *
 *				\brief Función principal, en esta función se realiza la conexión a través de sockets a través del procolo HTTP, puerto 80.
 *			@param Los parámetros introducidos:
 *					- ServerName, Archivo, Minutos, Segundos: provienen del programa main.c
 *			\return Devuelve los minutos y los segundos.
 \verbatim
		struct sockaddr_in serverAddr;
		int	sockAddrSize;
		int	sFd;
		int	result;
		char	buffer[256];
		int	j=0;
		int	i = 0;
		int content = 0;
		char data_information[20];
		char *time_lectura;
		char *time_alarma;
		char delimitador[2] = ",";
		@brief Crear el socket
			@param	sFd=socket(AF_INET,SOCK_STREAM,0);
		@brief Construir la dirección
			@note Las variables que están descritas anteriormente vienen referidas a struct sockaddr_in serverAddr
				@param sockAddrSize = sizeof(struct sockaddr_in)
				@param bzero ((char *)&serverAddr, sockAddrSize) @note Poner la estrucura a 0
				@param serverAddr.sin_family=AF_INET;
				@param serverAddr.sin_port=htons (SERVER_PORT_NUM);
				@param serverAddr.sin_addr.s_addr = inet_addr(serverName);

		@brief Conexión y seguridad de funcionamiento
			result = connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
			if (result < 0){
			printf("Error al establecer la conexión\n");
			exit(-1);
			}
			printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

		//Enviar
		sprintf(buffer,"GET /%s HTTP/1.1\r\n"\
					"Host: %s \r\n\r\n",Archivo,ServerName);
		result = write(sFd, buffer, strlen(buffer));
		printf("Missatge enviat a servidor(bytes %d): %s\n", result, buffer);

		//Recibir
		memset(buffer, 0, sizeof(buffer[0]));
		do{
	result = read(sFd, buffer, sizeof(buffer));
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer);
	}while(buffer == NULL);

		memset(buffer, '\0', sizeof(buffer[0]));
		\verbatim
			@note Apartir de la respuesta del archivo config.txt, separar la información que se requiere (segundos y minutos)
				content = buffer[161] - '0';
				i = result - content;
				for (j = 0; i < result; i++,j++){
				data_information[j] = buffer[i];
			}
		\endverbatim
		time_lectura = strtok(data_information, delimitador);
		time_alarma = strtok(NULL, delimitador);
		*segundos = atoi(time_lectura);
		*minutos = atoi(time_alarma);
		memset(buffer, '\0', sizeof(buffer[0]));
		close(sFd);
		return 0;
	}

 \endverbatim
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "http.h"


#define REQUEST_MSG_SIZE 1024
#define REPLY_MSG_SIZE  512
#define SERVER_PORT_NUM 80

int web_http(char ServerName[25], char *Archivo, int *minutos, int *segundos) {

	struct sockaddr_in serverAddr;
	int	sockAddrSize;
	int	sFd;
	int	result;
	char	buffer[256];
	int	j = 0;
	int	i = 0;
	int content = 0;
	char data_information[20];
	char *time_lectura;
	char *time_alarma;
	char delimitador[2] = ",";

	//Crear el socket
	sFd = socket(AF_INET, SOCK_STREAM, 0);
	//Construir la dirección
	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *)&serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = inet_addr(ServerName);
	//Conexion
	result = connect(sFd, (struct sockaddr *) &serverAddr, sockAddrSize);
	if (result < 0) {
	 printf("Error en establir la connexió\n");
	 exit(-1);
	}
	printf("\nConnexió establerta amb el servidor: adreça %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
	//Enviar
	sprintf(buffer, "GET /%s HTTP/1.1\r\n"\
					"Host: %s \r\n\r\n", Archivo, ServerName);
	result = write(sFd, buffer, strlen(buffer));
	printf("Missatge enviat a servidor(bytes %d): %s\n", result, buffer);
	//Recibir
	memset(buffer, 0, sizeof(buffer[0]));
	do {
	result = read(sFd, buffer, sizeof(buffer));
	printf("Missatge rebut del servidor(bytes %d): %s\n", result, buffer);
	} while (buffer == NULL);
    memset(buffer, '\0', sizeof(buffer[0]));
    content = buffer[147] - '0';
	i = result - content;
    for (j = 0; i < result; i++, j++) {
	data_information[j] = buffer[i];
	}
    time_lectura = strtok(data_information, delimitador);
    time_alarma = strtok(NULL, delimitador);
    *segundos = atoi(time_lectura);
    *minutos = atoi(time_alarma);
     memset(buffer, '\0', sizeof(buffer[0]));
	close(sFd);
	return 0;
}

