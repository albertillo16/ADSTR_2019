/**
 * @file			hum_temp.c
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 *
 */

/**
 * @file				hum_temp.c
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
 * @file				hum_temp.c
 *
 *						Descripción programa
 *					-----------------------------------------------
 * El programa realiza dos partes independientes:
 *				1. Muestra la ayuda cuando el usuario se ha equivocado en poner algo por el bash, esta parte esta referida en el programa main.c
 *				2. Ejecuta la lectura de la humedad y la temperatura cada xx segundos.
 */
 /**
 * @file				hum_temp.c
 *
 *						Librerias a incluir en el programa
 *						-----------------------------------------------
 \verbatim
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
		#include <linux/fb.h>
		#include <time.h>
		#include "hum_temp.h"
 \endverbatim
 */

 /**
 * @file				hum_temp.c
 *
 *						Variables predefinidas en el programa
 *						-----------------------------------------------
 \verbatim

			\def #define DEV_PATH "/dev/i2c-1"
			@param Activar comunicación i2c
			\def #define DEV_ID 0x5F
			@param Activar el registro para el sensor de humedad (HTS221).

	\def #define WHO_AM_I 0x0F
			@param Registro de solo lectura que contiene el identificador del dispositivo.

		\def #define CTRL_REG1 0x20
			@param Activado el bit PD, se utiliza para encender el dispositivo.
			@param El dispositivo está en modo apagado cuando PD = "0". (Valor predeterminado despúes del arranque).
			@param El dispositivo se activa cuando el PD se establece a "1".

		\def #define CTRL_REG2 0x21
			@param El bit 0, ONE_SHOT se usar para comenzar una nueva conversión.
			@param En esta situación, una sola adquisición de la temperatura y la humedad se inician cuando el bit ONE_SHOT se establece a 1.
			@param Al final de conversión, los nuevos datos están disponibles en los registros de salida, STATUS_REG[0] y los bits STATUS_REG[1] se establecen en "1" y el bit ONE_SHOT vuelve a "0" por hadware.

		\def #define STATUS_REG 0x27
			@param Se actualiza cada lectura de una sola vez de cada ciclo.
			@param El bit 1 del sensor de humedad, estará a 1 cuando el dato estará disponible.

		@note Los datos de humedad se expresan como HUMIDITY_OUT_H y HUMIDITY_OUT_L en 2 complementos.
			\def #define H_T_OUT_L 0x28
				@param Datos de humedad LSB
			\def #define H_T_OUT_H 0x29
				@param Datos de humedad MSB

		@note Los datos de temperatura se expresan como TEMP_OUT_L y TEMP_OUT_H en 2 complementos.
			\def #define TEMP_OUT_L 0x2A
				@param Datos de temperatura LSB
			\def #define TEMP_OUT_H 0x2B
				@param Datos de temperatura LSB

		@note Los registros en el rango de direcciones 0x30 a 0x3F contienen coeficinetes de calibración.
		@note Cada módulo del sensor tiene coeficiente individuales.
		@note Antes del primer cálculo de temperatura y humedad, el Master lee los coeficientes de calibración.
			\def #define H0_rH_x2 0x30
			\def #define H1_rH_x2 0x31
			\def #define T0_degC_x8 0x32
			\def #define T1_degC_x8 0x33
			\def #define T1_T0_MSB 0x35
			\def #define H0_T0_OUT_L 0x36
			\def #define H0_T0_OUT_H 0x37
			\def #define H1_T0_OUT_L 0x3A
			\def #define H1_T0_OUT_H 0x3B
			\def #define T0_OUT_L 0x3C
			\def #define T0_OUT_H 0x3D
			\def #define T1_OUT_L 0x3E
			\def #define T1_OUT_H 0x3F
 \endverbatim
*/

/**
 ** @file				hum_temp.c
 *
 *						Variables a utilizar en el programa
 *						-----------------------------------------------
 *
 \verbatim
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

 \endverbatim
 */

 /**
  * \fn void help(char *programa)
  *		\brief Función normalizada para mostrar la ayuda por el bash.
  *		Esta función servirá para saber que se debe de poner y tambien si el usuario se ha equivocado.
  *		@param Los parametros introducidos son propios del programa main.c
  *		\return No devuelve ningún valor.
  *
  * \fn int Humidity_temperature_sensor(float *humidity, float *temperature)
  *		\brief Función normalizada para la lectura de humedad y temperatura.
  *		@param Los parametros introducidos son propios del programa main.c
  *		\return Devuelve humedad y temperatura a través de punteros.
  *
  * \fn int  Matriz_led()
  *		\brief Función normalizada para visualizar que se ha hecho la lectura de la humedad y la temperatura.
  *		@param Los parametros introducidos son propios de esta función normalizada.
  *		\return No devuelve ningún valor
  *
  * \fn int Fecha_hora(char *tiempo_anterior, char *tiempo_actual, int minutos)
  *		\brief Función normalizada para visualizar la hora y la fecha en la que se ha hecho la captura y guardarla en la base de datos.
  *		Esta función a parte de tener la fecha  y la hora de la captura tiene otra fecha donde se le resta los minutos para la tabla de alarmas,
  *		de esta manera se obtiene los valores, la fecha y hora cuando se ha cometido.
  *		\return Devuelve la fecha - hora actual y fecha - hora anterior (pasado tiempo mostrar_alarmas).
  *
  */



 /**
  *
  * \fn void help(char *programa){
		\verbatim
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
			fprintf (stderr, "\t-a\tPresenta la dirección web\n\n");
			fprintf (stderr, "\t-b\tPresenta el archivo de la dirección web [config.txt]\n");
			fprintf (stderr, "\t-c\tPresenta la ruta del archivo de la base de datos [home/user/Documents/captura/basededatos.db]\n");
		exit (2);
		}
	\endverbatim
  */
 /**
  * \fn int Humidity_temperature_sensor(float *humidity, float *temperature){
		\verbatim
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

				@note Configuración I2C esclavo
					if (ioctl(fd,I2C_SLAVE,DEV_ID) < 0){
						perror("Error al configurar el dispositivo esclavo I2C, sensor HUMEDAD");
						close(fd);
						exit(1);
					}
				@note Se comprueba si el sensor es cual es (HTS221)
					if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBC) {
						close(fd);
						exit(1);
						}
				@note Se apaga el dispositivo para el inicio limpio
					i2c_smbus_write_byte_data(fd,CTRL_REG1,0x00);

				@note Se enciende la parte frontal del sensor de humedad en disparo único
					i2c_smbus_write_byte_data(fd,CTRL_REG1,0x84);
					*
				@note Se ejecuta la medición de disparo (humedad y temperatura)
				@note El bit establecido quiere ser restablecido por el sensoor en si mismo despues de la ejecución de autolimpieza.
					i2c_smbus_write_byte_data(fd,CTRL_REG2,0x01);

				@note Se ejecutar un do-while, donde se ejecuta un tiempo ya establecido de 20ms
				@note La variable estado, evalua si con los registros es igual a 1. Si es igual a 1 saldrá del bucle, dado que ha hecho una nueva conversión.
				@note Se mira el registros CTRL_REG2, STATUS_REG, 0x02 y 0x01.
					do {
						usleep(20000);
						estado_hum =  i2c_smbus_read_byte_data(fd, CTRL_REG2) && i2c_smbus_read_byte_data(fd, STATUS_REG) && 0x02;
						estado_temp = i2c_smbus_read_byte_data(fd, CTRL_REG2) && i2c_smbus_read_byte_data(fd, STATUS_REG) && 0X01;
					} while ((estado_hum != 0) && (estado_temp != 0));

				@note Leer datos de calibración de humedad relativa y temperatura  LSB y MSB (ADC) para dos puntos
					h0_out_l = i2c_smbus_read_byte_data(fd, H0_T0_OUT_L);
					h0_out_h = i2c_smbus_read_byte_data(fd, H0_T0_OUT_H);
					h1_out_l = i2c_smbus_read_byte_data(fd, H1_T0_OUT_L);
					h1_out_h = i2c_smbus_read_byte_data(fd, H1_T0_OUT_H);
					t0_out_l = i2c_smbus_read_byte_data(fd, T0_OUT_L);
					t0_out_h = i2c_smbus_read_byte_data(fd, T0_OUT_H);
					t1_out_l = i2c_smbus_read_byte_data(fd, T1_OUT_L);
					t1_out_h = i2c_smbus_read_byte_data(fd, T1_OUT_H);
				@note Leer datos y de calibración de humedad relativa (%rH) y temperatura para dos puntos
					h0_rh_x2 = i2c_smbus_read_byte_data(fd, H0_rH_x2);
					h1_rh_x2 = i2c_smbus_read_byte_data(fd, H1_rH_x2);
					t0_degC_x8 = i2c_smbus_read_byte_data(fd, T0_degC_x8);
					t1_degC_x8 = i2c_smbus_read_byte_data(fd, T1_degC_x8);
					t1_t0_msb = i2c_smbus_read_byte_data(fd, T1_T0_MSB);
				@note Desplazamiento de 16 bits (valores de calibración de humedad y temperatura)
					H0_T0_OUT = h0_out_h << 8 | h0_out_l;
					H1_T0_OUT = h1_out_h << 8 | h1_out_l;
					T0_OUT = t0_out_h << 8 | t0_out_l;
					T1_OUT = t1_out_h << 8 | t1_out_l;
				@note Desplazamiento de 16 bits a 10 con mascara
					T0_DegC_x8 = (t1_t0_msb & 3) << 8 | t0_degC_x8;
					T1_DegC_x8 = ((t1_t0_msb & 12) >> 2) << 8 | t1_degC_x8;
				@note Se divide entre 2 los valores obtenidos h0_rh_x2 y h1_rh_x2. Los nuevos valores son denominados H0_rH y H1_rH
					H0_rH = h0_rh_x2 / 2.0;
					H1_rH = h1_rh_x2 / 2.0;
				@note Se divide entre 8 los valores obtenidos T0_DegC_x8 y T1_DegC_x8. Los nuevos valores son denominados T0_rH y T1_rH
					T0_DegC = T0_DegC_x8 / 8.0;
					T1_DegC = T1_DegC_x8 / 8.0;

		\brief Fórmula de la ecuación de la recta para cálcular la humedad relativa
				@note %rH = ((H1_rH - H0_rH) · (H_T_OUT - H0_T0_OUT) / (H1_T0_OUT - H0_T0_OUT)) + H0_rH
				@note Gradiente (inclinación de la recta)
				@param  h_gradient_m = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
				@note Interceptar
				@param h_intercept_c = H1_rH - (h_gradient_m * H1_T0_OUT);

					h_gradient_m_H = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
					h_intercept_c_H = H1_rH - (h_gradient_m_H * H1_T0_OUT);

		\brief Fórmula de la ecuación de la recta para cálcular la temperatura
				@note ºC  = ((T1_DegC -T10_DegC) · (T1_OUT - T0_OUT) / (T1_T0_OUT - T0_T0_OUT)) + T0_DegC
				@note Gradiente (inclinación de la recta)
				@param t_gradient_m_T = (T1_DegC - T0_DegC) / (T1_OUT - T0_OUT);
				@note Interceptar
				@param t_intercept_c_T = T1_DegC - (t_gradient_m_T * T1_OUT);

					uint8_t h_t_out_l = i2c_smbus_read_byte_data(fd, H_T_OUT_L);
					uint8_t h_t_out_h = i2c_smbus_read_byte_data(fd, H_T_OUT_H);
					uint8_t t_out_l = i2c_smbus_read_byte_data(fd, TEMP_OUT_L);
					uint8_t t_out_h = i2c_smbus_read_byte_data(fd, TEMP_OUT_H);

				@note Desplazamiento de 16 bits (valores de calibración de humedad y temperatura)
					int16_t H_T_OUT = h_t_out_h << 8 | h_t_out_l;
					int16_t T_OUT = t_out_h << 8 | t_out_l;

				@note Guardar los valores a sus respectivas variables
					*humidity = (h_gradient_m_H * H_T_OUT) + h_intercept_c_H;
					*temperature = (t_gradient_m_T * T_OUT) + t_intercept_c_T;
				@note Llamar a la función Matriz_led para visualizar que ha hecho la captura.
					Matriz_led();
				@note Se apaga el dispositivo y se cierra el descriptor de comunicación I2C
					i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);
					close(fd);
					return 0;
					}

		\brief Conversión de humedad del ADC_OUT a %rH
		\brief Interpolación lineal para humedad relativa (ejemplo):

		@note Input: humedad relativa LSB (ADC)				@note OUTPUT: Humedad relativa (% rH)

		@note H0_T0_OUT = 0x4000							@note H0_rH_x2 = 40% rH --> 40/2 = 20.0% rH

		@note H1_T0_OUT = 0x6000							@note H1_rH_x2 = 80% rH --> 80/2 = 40.0% rH

		@note Conversion de humedad

		@note H_OUT = 0x5000								@note H_rH_x2 = 60% [interpolación] --> 60/2 = 30.0% rH

	\endverbatim
 */

 /**
  *\fn int  Matriz_led(){
	 \verbatim
		#include <linux/fb.h>
		int i;
		int fbfd = 0;
		uint16_t *map;
		struct fb_fix_screeninfo fix_info;

		\param uint16_t *map y uint16_t *p;
			@note Punteros de 16 bits.
		\struct fb_fix_screeninfo fix_info;
		\param fb_fix_screeninfo
				@note Proporciona el estado actual del hardware de video.
				@note Define las propiedades no modificables de una targeta grafica que se configuran cuando cambia la resolución. Un buen ejemplo
				@note es el inicio de la memoria framebuffer. Es posible que esta dirección dependa de la resolución, endianes o profundidad de color
				@note a la que se ejecuta la targeta gráfica. Ahora, mientas se usa ese modo, no se desea que cambie la posición de la memoria. En este caso,
				@note el hardware de gráficos le indica la ubicación de la memoria y no tiene nada que decir al respecto.
			\param fix_info
				@note Puntero donde se le guardará un valor. Si en el caso que de -1, el driver no se ha podido cargar.


		fbfd = open(FILEPATH, O_RDWR);

		if (fbfd == -1) {
			perror("Error al abrir el dispositivo");
			exit(EXIT_FAILURE);
		}

		@note Se obtiene la información sobre las estructuras a través de las llamadas ioct  sobre el dispositivo, como por ejmeplo el formato de cada pixel, las capacidades de la targeta,
		asi como cambiar el modo gráfico.

		if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
			perror("Error (llamada del ioctl");
			close(fbfd);
			exit(EXIT_FAILURE);
		}

		@note Se comprueba que se ha encontrado el dispotivo correcto, el registros 0x46
		if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
			printf("%s\n", "Error: RPI-Sense FB no se encuentra");
			close(fbfd);
			exit(EXIT_FAILURE);
		}
		\brief La función mmap intentar ubicar la longitud de bytes comenzando en el desplazamiento offset del fichero
		\brief u otro objeto especificado por el descriptorio de fichero fbfd en memoria.
		\brief El lugar donde es ubicado el objeto es devuelto por mmap, y nunca vale 0.
			@note MAP_SHARED
			@note Comparte esta área con todos los otros objetos que señalan a este objeto. Almacenar en la región es equivalente
			@note a escribir en el fichero. El fichero puede no actualizarse hasta que se llame a msync o munmap.
			@note PROT_xxx
			@note El argumento PROT describe la protección de memoria deseada, no debe entrar en conflicto con el modo de apertura del fichero.
					@param	PROT_READ
					@param	PROT_WRITE
		map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
		if (map == MAP_FAILED) {
			close(fbfd);
			perror("Error al mapear el archivo");
			exit(EXIT_FAILURE);
		}

		memset(map, 0, FILESIZE);
		for (i = 0; i < 3; i++) {
				usleep(25000);
				memset(map, 0x86, FILESIZE);
				usleep(25000);
				memset(map, 0, FILESIZE);
		}
		memset(map, 0, FILESIZE);

		if (munmap(map, FILESIZE) == -1) {
			perror("Error al desasignar el archivo");
		}
		close(fbfd);

		return 0;
		}

	\endverbatim
  */

 /**
  * \fn int Fecha_hora(char *tiempo_anterior, char *tiempo_actual, int minutos){
  \verbatim
			time_t tiempo = time(NULL);
			struct tm *tlocal = localtime(&tiempo);
			strftime(tiempo_actual,20,"%d/%m/%y %H:%M:%S",tlocal);

		@note Parte para poder coincidir la fecha de alarmas con la tabla de lecturas, por ello se resta el segun_min, es el valor que viene del HTTP
			time_t t_anterior;
			struct tm *fecha_anterior;
			t_anterior = time(NULL);
			t_anterior -= minutos;
			fecha_anterior = localtime(&t_anterior);
			strftime(tiempo_anterior,20,"%d/%m/%y %H:%M:%S",fecha_anterior);
	return 0;
  \endverbatim
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





void help(char *programa)
{
	fprintf(stderr, "IMPLEMENTATION\nGNU bash, version 1.0 - release (i686-pc-linux-gnu)\n"
					"Copyright (C) 2019 Free Software Foundation, Inc.\n"
					"License GPLv3: GNU GPL version 1 or later\n\n");
	fprintf(stderr, "RESUMEN\nEste programa conecta con los protocolos HTTP y SMTP\n"
					"El protocolo HTTP conecta con una pagina web donde estan instaurados unos numeros.\n"
					"Dichos numeros son los segundos de lectura y los minutos de listado de alarmas [ss,mm].\n"
					"[Segundos de lectura] sirve para ejecutar la lectura de humedad y temperatura.\n"
					"[Listado de alarmas] sirve para ejecutar cada cuanto que se envie el listado de alarmas.\n"
					"El correo STMP será para enviar los datos a un correo electronico\n\n");

		fprintf(stderr, "[INSTRUCCIONES]\nUso %s -h help [OPCIONES] [fichero]\n", programa);
		fprintf(stderr, "\t-?\tPresenta esta ayuda en pantalla.\n");
		fprintf(stderr, "\t-a\tPresenta la dirección web\n\n");
		fprintf(stderr, "\t-b\tPresenta el archivo de la dirección web [config.txt]\n");
		fprintf(stderr, "\t-c\tPresenta la ruta del archivo de la base de datos [home/user/Documents/captura/basededatos.db]\n");
	exit(2);
}

int Humidity_temperature_sensor(float *humidity, float *temperature)
{

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
	uint8_t t0_degC_x8 = 0;
	uint8_t t1_degC_x8 = 0;
	uint8_t t1_t0_msb = 0;
	uint16_t T0_DegC_x8 = 0;
	uint16_t T1_DegC_x8 = 0;
	int16_t H0_T0_OUT = 0;
	int16_t H1_T0_OUT = 0;
	int16_t T0_OUT = 0;
	int16_t T1_OUT = 0;

	double H0_rH  = 0;
	double H1_rH = 0;
	double T0_DegC = 0;
	double T1_DegC = 0;

	double h_gradient_m_H = 0;
    double h_intercept_c_H = 0;
    double t_gradient_m_T = 0;
    double t_intercept_c_T = 0;

	if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
		perror("Error al abrir el puerto I2C");
		exit(1);
	}

	//* Configuración I2C esclavo *//
	if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
		perror("Error al configurar el dispositivo esclavo I2C, sensor HUMEDAD");
		close(fd);
		exit(1);
	}
	if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBC) {
		close(fd);
		exit(1);
		}
	//* Apague el dispositivo (Inicio limpio) *//
	i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

	//* Enciende la parte frontal analógica del sensor de humedad de disparo único *//
	i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x84);

	//* Ejecuta la medición de disparo (humedad y temperatura). El bit estabalecido quiere ser el restablecido por el
	//* sensor en sí mismo despúes de la ejecución (bit de autolimpieza) *//
	i2c_smbus_write_byte_data(fd, CTRL_REG2, 0x01);

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

int  Matriz_led()
{
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
int Fecha_hora(char *tiempo_anterior, char *tiempo_actual, int minutos)
{

	time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    strftime(tiempo_actual, 20, "%d/%m/%y %H:%M:%S", tlocal);

   //Parte para poder coincidir la fecha de alarmas con la tabla de lecturas, por eso se resta el segun_min, es el valor que viene del HTTP
    time_t t_anterior;
    struct tm *fecha_anterior;

    t_anterior = time(NULL);
    t_anterior -= minutos;
    fecha_anterior = localtime(&t_anterior);
    strftime(tiempo_anterior, 20, "%d/%m/%y %H:%M:%S", fecha_anterior);
	return 0;

}
