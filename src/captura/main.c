/**
 * @file			main.c
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 *
 */






/**
 * @file				main.c
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
 * @file				main.c
 *
 *						Descripción programa
 *						-----------------------------------------------
 * El programa realiza una lectura de humedad y temperatura de la placa SENSE HAT. Los tiempos de lectura vienen dado por la libreria dinámica "http.h", donde se extraen dos números.
 * Los números antes de la coma, significan los segundo en los que tiene que hacer la lectura, y los números restantes los minutos para ver si ha habido una alarma.
 * Para cada valor de temperatura y humedad se guardarán en una base de datos, como tambien si ha ocurrido una alarma.
 * El programa consta de  4 librerias dinámicas:
 *				- Hum_temp.h : Lectura de humedad y temperatura cada x segundos.
 *				- http.h : página web, donde se extraen los segundos (captura) y los minutos (alarmas).
 *				- basededatos.h : Lugar donde se guardarán las medidas del sensor, como las alarmas si las ha habido.
 *
 * El programa comparte información con el programa de informe, donde dicho programa ejecuta un código para enviar un correo a tal dirección. Se ejecutará una vez por día a las 23:59 mediante el
 * servició CRON.
 *
 */

/**
 * @file				main.c
 *
 *						Librerias a incluir en el programa
 *						-----------------------------------------------
 \verbatim
		#include <string.h>
		#include <stdio.h>
		#include <stdlib.h>
		#include <ctype.h>
		#include <ctype.h>
		#include <unistd.h>
		#include "hum_temp.h"
		#include "http.h"
		#include "basededatos.h"
 \endverbatim
 */

/**
 ** @file				main.c
 *
 *						Variables a utilizar en el programa
 *						-----------------------------------------------
 *
 \verbatim
		char *ServerName  = NULL;
		char *Archivo= NULL;
		char *rutabase_datos = NULL;
		int aflag = 0;
		int bflag = 0;
		int cflag = 0;
		int hflag = 0;
		int index = 0;
		int c = 0;
		int segundos = 0;
		int minutos = 0;
		char tiempo_anterior[20],tiempo_actual[20];
		int contadorseg = 0;
		int contadormin = 0;
		float temperature = 0; float humidity = 0;
 \endverbatim
 */
 /**
 ** @file				main.c
 *
 *						Llamadas a funciones - librerias dinámicas
 *						-----------------------------------------------
 *
	\fn web_http(ServerName, Archivo, &minutos, &segundos);
	\fn crear_basededatos(rutabase_datos);
	\fn Fecha_hora(tiempo_anterior,tiempo_actual, minutos);
	\fn Humidity_temperature_sensor(&humidity, &temperature);
	\fn insertar_basededatos(rutabase_datos, &temperature, &humidity, tiempo_actual);
	\fn alarm_basededatos(rutabase_datos, tiempo_anterior);
 *
 *
 */

/**
 * @file	main.c
 *
 *						Compilación y ejecución del programa captura
 *						-----------------------------------------------
 *
 *
 * Makefile
 *
 * Compilación:	make captura
 *
 * Ejecución:		make all
 */

 /**
  *
  * \fn int main(int argc, char **argv);
  *
  *			\brief Función principal, en esta función se realiza las distintas consultas a las librerias dinámicas.
  *			@param Los parámetros introducidos son própios de esta función normalizada, cada parámetro se pasa a través de puntero a
  *			alguna libreria dinámica.
  *			\return No devuelve ningún valor.
 \verbatim
	\verbatim
		@brief La función getopt () analiza los argumentos de la línea de comandos. Sus argumentos argc y argv son el recuento de argumentos y la matriz
		que se pasa a la función main () en la invocación del programa. Un elemento de argv que comienza con '-' (y no exactamente "-" o "-" ) es una opción elemento.
		Los caracteres de este elemento (aparte del '-' inicial) son caracteres opcionales. si se llama getopt() repetidamente, devuelve sucesivamemte cada uno de los caracteres
		de opciñon de cada una de las opciones de elementos.
		@param Optarg: Se guarda el elemento de argv en optarg.
		@param help(argv[0]): muestra la información de ayuda por pantalla.

		char *ServerName  = NULL;char *Archivo= NULL;char *rutabase_datos = NULL;
		int aflag = 0;int bflag = 0;int cflag = 0;int hflag = 0;
		int index = 0;
		int c = 0;
		int segundos = 0;
		int minutos = 0;
		char tiempo_anterior[20],tiempo_actual[20];
		int contadorseg = 0;
		int contadormin = 0;
		float temperature = 0; float humidity = 0;
		while ((c = getopt (argc, argv, "a:b:c:h:")) != -1)
			switch (c){
			case 'a':
			aflag = 1;
			ServerName = optarg;
			printf("Aflag = %d - IP = %s\n", aflag, ServerName);
			break;
			case 'b':
			bflag = 1;
			Archivo = optarg;
			printf("Bflag = %d - Archivo = %s\n", bflag, Archivo);
			break;
			case 'c':
			cflag = 1;
			rutabase_datos = optarg;
			printf("Cflag = %d - Ruta= %s\n", cflag, rutabase_datos);
			break;
			case 'h':
				hflag = 1;
				printf("Hflag = %d",hflag);
				help(argv[0]);
				break;
				case '?':
					if (strchr("abch", optopt) != NULL)
						fprintf (stderr, "La opción -%c requiere un argumento.\n", optopt);
					else if (isprint (optopt))
						fprintf (stderr, "Opción desconocida'-%c'.\n", optopt);
					else
					fprintf (stderr, "Carácter no válido '\\x%x'.\n", optopt);
				help(argv[0]);
			default:
			abort ();
		}
		for (index = optind; index < argc; index++){
		printf ("Argumento sin opción %s\n", argv[index]);
		}
	\endverbatim
	web_http(ServerName, Archivo, &minutos, &segundos);
	crear_basededatos(rutabase_datos);
	minutos = minutos * 60;
	while(1){
	contadorseg++;
	contadormin++;
		if (contadorseg == segundos){
			Fecha_hora(tiempo_anterior,tiempo_actual, minutos);
			Humidity_temperature_sensor(&humidity, &temperature);
			insertar_basededatos(rutabase_datos, &temperature, &humidity, tiempo_actual);
			contadorseg =0;
		}
		if (contadormin == minutos){
			contadormin = 0;
			alarm_basededatos(rutabase_datos, tiempo_anterior);
		}
	sleep(1);
	}
	return 0;
	}
 \endverbatim
  */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ctype.h>
#include <unistd.h>
#include "hum_temp.h"
#include "http.h"
#include "basededatos.h"

int main(int argc, char **argv)
{
	char *ServerName  = NULL; char *Archivo = NULL; char *rutabase_datos = NULL;
	int aflag = 0; int bflag = 0; int cflag = 0; int hflag = 0;
	int index = 0;
	int c = 0;
	int segundos = 0;
	int minutos = 0;
	char tiempo_anterior[20], tiempo_actual[20];
	int contadorseg = 0;
	int contadormin = 0;
	float temperature = 0; float humidity = 0;

	while ((c = getopt(argc, argv, "a:b:c:h:")) != -1)
		switch (c) {
		case 'a': // IP
		aflag = 1;
		ServerName = optarg;
		printf("Aflag = %d - IP = %s\n", aflag, ServerName);
		break;
		case 'b':
		bflag = 1;
		Archivo = optarg;
		printf("Bflag = %d - Archivo = %s\n", bflag, Archivo);
		break;
		case 'c':
		cflag = 1;
		rutabase_datos = optarg;
		printf("Cflag = %d - Ruta= %s\n", cflag, rutabase_datos);
		break;
		case 'h':
				hflag = 1;
				printf("Hflag = %d", hflag);
				help(argv[0]);
				break;
				case '?':
					if (strchr("abch", optopt) != NULL)
						fprintf(stderr, "La opción -%c requiere un argumento.\n", optopt);
					else if (isprint (optopt))
						fprintf(stderr, "Opción desconocida'-%c'.\n", optopt);
					else
					fprintf(stderr, "Carácter no válido '\\x%x'.\n", optopt);
				help(argv[0]);
	default:
	abort();
	}
	for (index = optind; index < argc; index++) {
	  printf("Argumento sin opción %s\n", argv[index]);
	}
	web_http(ServerName, Archivo, &minutos, &segundos);
	printf("Los minutos son: %d\t\n", minutos);
	printf("Los segundos son: %d\t\n", segundos);
	crear_basededatos(rutabase_datos);
    minutos = minutos * 60;
    while (1) {
	contadorseg++;
	contadormin++;
			if (contadorseg == segundos) {
			Fecha_hora(tiempo_anterior, tiempo_actual, minutos);
			Humidity_temperature_sensor(&humidity, &temperature);
			insertar_basededatos(rutabase_datos, &temperature, &humidity, tiempo_actual);
			contadorseg = 0;				
			}
			if (contadormin == minutos) {
			contadormin = 0;
			alarm_basededatos(rutabase_datos, tiempo_anterior);
			}
	sleep(1);
	}
	return 0;
}

