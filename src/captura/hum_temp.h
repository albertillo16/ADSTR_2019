/**
 * @file			hum_temp.h 
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 * 
 */
 
/**
 * @file				hum_temp.h
 * 						Copyright
 * 						-----------------------------------------------
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
	@note Libreria dinámica para comunicarse con el SENSE HAT mediante I2C (sensor humedad, temperatura y matriz led).
	@note Mostrar fecha y hora anterior y actual.
	@note Ayuda para el usuario del bash.
		#ifndef HUM_TEMP_H
		#define HUM_TEMP_H
		void help(char *programa);
		int  Matriz_led();
		int Humidity_temperature_sensor(float *humidity, float *temperature);
		int Fecha_hora(char *tiempo_anterior, char *tiempo_actual, int minutos);
		#endif
 */
#include <stdint.h>

#ifndef HUM_TEMP_H
#define HUM_TEMP_H
void help(char *programa);
int  Matriz_led();
int Acc_sensor(float *acc_Yangle);
void readBlock(uint8_t command, uint8_t size, uint8_t *data, int fx);
int Humidity_temperature_sensor(float *humidity, float *temperature);
int Fecha_hora(char *tiempo_anterior, char *tiempo_actual, int minutos);
#endif
	
