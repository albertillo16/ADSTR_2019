/**
 * @file			http.h 
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 * 
 */
 
/**
 * @file				http.h
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
 * @note Libreria dinámica para comunicarse con el protocolo HTTP.
	#ifndef HTTP_H
		#define HTTP_H
		int web_http(char ServerName[25], char* Archivo, int *minutos, int *segundos);
		#endif
 */
#ifndef HTTP_H
#define HTTP_H
int web_http(char ServerName[25], char *Archivo, int *minutos, int *segundos);
#endif
