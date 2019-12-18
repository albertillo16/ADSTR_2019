/*
 * main.c
 * 
 * Copyright 2019 Alberto <alberto@alberto-VirtualBox>
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
 * 
 * 
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <ctype.h>
#include <unistd.h>
#include "hum_temp.h"
#include "http.h"
#include "inicio_letras.h"
#include "basededatos.h"

int main(int argc, char **argv)
{
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
	//int conta_seg = 0;
	while ((c = getopt (argc, argv, "a:b:c:h:")) != -1)
		switch (c){
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
	inicio_letras();
	web_http(ServerName, Archivo, &minutos, &segundos);
	printf("Los minutos son: %d\t\n", minutos);
	printf("Los segundos son: %d\t\n",segundos);
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

