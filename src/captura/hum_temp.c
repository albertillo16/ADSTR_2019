/*
 * hum_temp.c
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
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "hum_temp.h"

#define DEV_PATH "/dev/i2c-1"
#define DEV_ID 0x5F
#define WHO_AM_I 0x0F
#define CTRL_REG1 0x20  
#define CTRL_REG2 0x21  
#define STATUS_REG 0x27

#define H0_T0_OUT_L 0x36
#define H0_T0_OUT_H 0x37
#define H1_T0_OUT_L 0x3A
#define H1_T0_OUT_H 0x3B
#define H0_rH_x2 0x30
#define H1_rH_x2 0x31
#define H_T_OUT_L 0x28	
#define H_T_OUT_H 0x29

#define T0_OUT_L 0x3C		
#define T0_OUT_H 0x3D
#define T1_OUT_L 0x3E
#define T1_OUT_H 0x3F		
#define T0_degC_x8 0x32
#define T1_degC_x8 0x33
#define T1_T0_MSB 0x35		

#define TEMP_OUT_L 0x2A		
#define TEMP_OUT_H 0x2B	

#define FILEPATH "/dev/fb1"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))





void help(char *programa){
	fprintf (stderr, "IMPLEMENTATION\nGNU bash, version 1.0 - release (i686-pc-linux-gnu)\n"
					"Copyright (C) 2019 Free Software Foundation, Inc.\n" 
					"License GPLv3: GNU GPL version 1 or later\n\n");
	fprintf (stderr, "RESUMEN\nEste programa conecta con los protocolos HTTP y SMTP\n"
					"El protocolo HTTP conecta con una pagina web donde estan instaurados unos numeros.\n"
					"Dichos numeros son los segundos de lectura y los minutos de listado de alarmas [ss,mm].\n"
					"[Segundos de lectura] sirve para ejecutar la lectura de humedad y temperatura.\n"
					"[Listado de alarmas] sirve para ejecutar cada cuanto que se envie el listado de alarmas.\n"
					"El correo STMP será para enviar los datos a un correo electronico\n\n");
					
		fprintf (stderr, "[INSTRUCCIONES]\nUso %s -h help [OPCIONES] [fichero]\n", programa);
		fprintf (stderr, "\t-?\tPresenta esta ayuda en pantalla.\n");
		fprintf (stderr, "\t-a\tPresenta la dirección web \n\n");
		fprintf (stderr, "\t-b\tPresenta el archivo de la dirección web [config.txt]\n");
		fprintf (stderr, "\t-c\tPresenta la ruta del archivo de la base de datos [home/user/Documents/captura/basededatos.db]\n");
	exit (2);
}

int Humidity_temperature_sensor(float *humidity, float *temperature){
	
	int fd = 0;
	uint8_t estado_hum = 0;
	uint8_t estado_temp = 0;
	uint8_t h0_out_l = 0;
	uint8_t h0_out_h = 0;
	uint8_t h1_out_l = 0;
	uint8_t h1_out_h = 0;
	uint8_t t0_out_l = 0;
	uint8_t t0_out_h = 0;
	uint8_t t1_out_l = 0;
	uint8_t t1_out_h = 0;
	
	uint8_t h0_rh_x2 = 0;
	uint8_t h1_rh_x2 = 0;
	uint8_t t0_degC_x8 =0;
	uint8_t t1_degC_x8 = 0;
	uint8_t t1_t0_msb = 0;
	uint16_t T0_DegC_x8 = 0;
	uint16_t T1_DegC_x8 = 0;
	int16_t H0_T0_OUT = 0;
	int16_t H1_T0_OUT = 0;
	int16_t T0_OUT =0;
	int16_t T1_OUT = 0;
	
	double H0_rH  = 0;
	double H1_rH = 0;
	double T0_DegC = 0;
	double T1_DegC = 0;
	
	double h_gradient_m_H = 0;
    double h_intercept_c_H = 0;
    double t_gradient_m_T = 0;
    double t_intercept_c_T = 0;
    
	if ((fd = open(DEV_PATH, O_RDWR)) < 0){
		perror("Error al abrir el puerto I2C");
		exit(1);
	}
	
	//* Configuración I2C esclavo *//
	if (ioctl(fd,I2C_SLAVE,DEV_ID) < 0){
		perror("Error al configurar el dispositivo esclavo I2C, sensor HUMEDAD");
		close(fd);
		exit(1);	
	}
	if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBC) {
		close(fd);
		exit(1);
		}	
	//* Apague el dispositivo (Inicio limpio) *//
	i2c_smbus_write_byte_data(fd,CTRL_REG1,0x00);
	
	//* Enciende la parte frontal analógica del sensor de humedad de disparo único *//
	i2c_smbus_write_byte_data(fd,CTRL_REG1,0x84);
	
	//* Ejecuta la medición de disparo (humedad y temperatura). El bit estabalecido quiere ser el restablecido por el
	//* sensor en sí mismo despúes de la ejecución (bit de autolimpieza) *//
	i2c_smbus_write_byte_data(fd,CTRL_REG2,0x01);
	
	do {
		usleep(20000);   //* 25 milisegundos
		estado_hum =  i2c_smbus_read_byte_data(fd, CTRL_REG2) && i2c_smbus_read_byte_data(fd, STATUS_REG) && 0x02;
		estado_temp = i2c_smbus_read_byte_data(fd, CTRL_REG2) && i2c_smbus_read_byte_data(fd, STATUS_REG) && 0X01;
	} while ((estado_hum != 0) && (estado_temp != 0));
	 
	
	h0_out_l = i2c_smbus_read_byte_data(fd, H0_T0_OUT_L);
	h0_out_h = i2c_smbus_read_byte_data(fd, H0_T0_OUT_H);
	h1_out_l = i2c_smbus_read_byte_data(fd, H1_T0_OUT_L);
	h1_out_h = i2c_smbus_read_byte_data(fd, H1_T0_OUT_H);
	t0_out_l = i2c_smbus_read_byte_data(fd, T0_OUT_L);
	t0_out_h = i2c_smbus_read_byte_data(fd, T0_OUT_H);
	t1_out_l = i2c_smbus_read_byte_data(fd, T1_OUT_L);
	t1_out_h = i2c_smbus_read_byte_data(fd, T1_OUT_H);
	
	h0_rh_x2 = i2c_smbus_read_byte_data(fd, H0_rH_x2);
	h1_rh_x2 = i2c_smbus_read_byte_data(fd, H1_rH_x2);
	t0_degC_x8 = i2c_smbus_read_byte_data(fd, T0_degC_x8);
	t1_degC_x8 = i2c_smbus_read_byte_data(fd, T1_degC_x8);
	t1_t0_msb = i2c_smbus_read_byte_data(fd, T1_T0_MSB);

	H0_T0_OUT = h0_out_h << 8 | h0_out_l;
	H1_T0_OUT = h1_out_h << 8 | h1_out_l;
	
	T0_OUT = t0_out_h << 8 | t0_out_l;
	T1_OUT = t1_out_h << 8 | t1_out_l;
	T0_DegC_x8 = (t1_t0_msb & 3) << 8 | t0_degC_x8;
	T1_DegC_x8 = ((t1_t0_msb & 12) >> 2) << 8 | t1_degC_x8;
	
	H0_rH = h0_rh_x2 / 2.0;
    H1_rH = h1_rh_x2 / 2.0;
	T0_DegC = T0_DegC_x8 / 8.0;
	T1_DegC = T1_DegC_x8 / 8.0;
	
    h_gradient_m_H = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
    h_intercept_c_H = H1_rH - (h_gradient_m_H * H1_T0_OUT);
	t_gradient_m_T = (T1_DegC - T0_DegC) / (T1_OUT - T0_OUT);
	t_intercept_c_T = T1_DegC - (t_gradient_m_T * T1_OUT);
	
    uint8_t h_t_out_l = i2c_smbus_read_byte_data(fd, H_T_OUT_L);
    uint8_t h_t_out_h = i2c_smbus_read_byte_data(fd, H_T_OUT_H);
    uint8_t t_out_l = i2c_smbus_read_byte_data(fd, TEMP_OUT_L);
	uint8_t t_out_h = i2c_smbus_read_byte_data(fd, TEMP_OUT_H);

  
    int16_t H_T_OUT = h_t_out_h << 8 | h_t_out_l;
    int16_t T_OUT = t_out_h << 8 | t_out_l;
  
    *humidity = (h_gradient_m_H * H_T_OUT) + h_intercept_c_H;
	*temperature = (t_gradient_m_T * T_OUT) + t_intercept_c_T;
	Matriz_led();
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);
    close(fd);
	
	return 0;
}

int  Matriz_led(){
	#include <linux/fb.h>
	int i;
	int fbfd = 0;
	uint16_t *map;
	struct fb_fix_screeninfo fix_info;

	fbfd = open(FILEPATH, O_RDWR);

	if (fbfd == -1) {
		perror("Error al abrir el dispositivo");
		exit(EXIT_FAILURE);
	}


	// Se puede obtener información sobre el dispositivo, como por ejemplo el formato de cada pixel, las capacidaddes de la targeta, asi como cambiar el moodo grafico
	//* Leer información de pantalla fija para el dispositivo abierto *//
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
		perror("Error (llamada del ioctl");
		close(fbfd);
		exit(EXIT_FAILURE);
	}

	//*	Comprueba que se ha encontrado el dispositivo correcto *//
	if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
		printf("%s\n", "Error: RPI-Sense FB no se encuentra");
		close(fbfd);
		exit(EXIT_FAILURE);
	}
	map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (map == MAP_FAILED) {
		close(fbfd);
		perror("Error al mapear el archivo");
		exit(EXIT_FAILURE);
	}
	memset(map, 0, FILESIZE);

	//*Limpiar la matriz de leds *//
	for (i = 0; i < 3; i++) {
			usleep(25000);
			memset(map, 0x86, FILESIZE);
			usleep(25000);
			memset(map, 0, FILESIZE);		
	}
	memset(map, 0, FILESIZE);
	//* Anular el mapa y cerrar *//
	if (munmap(map, FILESIZE) == -1) {
		perror("Error al desasignar el archivo");
	}
	close(fbfd);

	return 0;
}
int Fecha_hora(char *tiempo_anterior, char *tiempo_actual, int minutos){
	
	time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);
    strftime(tiempo_actual,20,"%d/%m/%y %H:%M:%S",tlocal);
		
   //Parte para poder coincidir la fecha de alarmas con la tabla de lecturas, por eso se resta el segun_min, es el valor que viene del HTTP
    time_t t_anterior;
    struct tm *fecha_anterior;
    t_anterior = time(NULL);
    t_anterior -= minutos;
    fecha_anterior = localtime(&t_anterior);
    strftime(tiempo_anterior,20,"%d/%m/%y %H:%M:%S",fecha_anterior);
	return 0;
	
}
