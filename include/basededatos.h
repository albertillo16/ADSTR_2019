/**
 * @file			basededatos.h 
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 * 
 */
 
/**
 * @file				basededatos.h
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
 * 	@note Libreria dinámica, se encarga de la comunicación con la base de datos.
 * 			- Crear
 * 			- Insertar
 * 			- Mostrar alarmas
 		#ifndef BASEDEDATOS_H
		#define BASEDEDATOS_H

		int crear_basededatos(char* rutabase_datos);
		int insertar_basededatos(char* rutabase_datos, float *humidity, float *temperature, char* tiempo_actual);
		int alarm_basededatos(char* rutabase_datos, char* tiempo_anterior);
		int callback_alarma(void *data, int argc, char **argv, char **azColName);
		int callback(void *NotUsed, int argc, char **argv, char **azColName);
	
		#endif
 */

#ifndef BASEDEDATOS_H
#define BASEDEDATOS_H

int crear_basededatos(char *rutabase_datos);
int insertar_basededatos(char *rutabase_datos, float *humidity, float *temperature, char* tiempo_actual);
int alarm_basededatos(char *rutabase_datos, char* tiempo_anterior);
int callback_alarma(void *data, int argc, char **argv, char **azColName);
int callback(void *NotUsed, int argc, char **argv, char **azColName);

#endif
