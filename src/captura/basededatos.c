/**
 * @file			basededatos.c
 * @brief
 * Begin : Martes Dic 17 2019
 * Group : A2
 * Classroom : AT-20
 * Signature : ADRST
 * Worktable : 05
 *
 */

/**
 * @file				basededatos.c
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
 * @file				basededatos.c
 *
 *						Descripción programa
 *						-----------------------------------------------
 El programa guarda los datos de temperatura, humedad y fecha en una base de datos.
 Este programa comparte información con el programa principal main.c y hum_temp.c
*/
/**
 * @file				basededatos.c
 *
 *						Librerias a incluir en el programa
 *						-----------------------------------------------
 \verbatim
		#include <stdio.h>
		#include <stdlib.h>
		#include <sqlite3.h>
		#include <string.h>
		#include "basededatos.h"
 \endverbatim
 */
 /**
  *
  * \fn int  crear_basededatos(char* rutabase_datos){
  *
  *	\brief Esta función se encarga de crear una base de datos en una ruta determinada y con un nombre.
  *	@param Los parametros introducidos provienene del programa main.c
  *	\return No devuelve ningún valor.
  *
	\verbatim
			sqlite3 *db;
			int rc;
			char *zErrMsg;
			char sql[256];

			rc = sqlite3_open(rutabase_datos,&db);

			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql,"CREATE TABLE Datos_sensor( " \
						"ID		INT ," \
						"TIPO		CHAR(300) ," \
						"DESCRIPCIÓN	CHAR(300) );");
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			memset(sql,'\0',sizeof(sql[0]));
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg); //escriu al bash
			sprintf(sql,"INSERT INTO Datos_sensor (ID,TIPO,DESCRIPCIÓN) "\
						"VALUES (1, 'Humedad', 'Humedad la placa solar.' );"\
						"INSERT INTO Datos_sensor(ID,TIPO,DESCRIPCIÓN)"\
						"VALUES (2, 'Temperatura', 'Temperatura ambiental');");
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql,"CREATE TABLE Datos_lecturas( " \
						"ID		INT,"\
						"Humedad	FLOAT ,"\
						"Temperatura	FLOAT ,"\
						"Unidad	CHAR(8) ,"
						"Fecha_hora_lectura		DATE );");
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql,"CREATE TABLE Datos_alarm( " \
						"Fecha_hora_alarm		DATE,"\
						"Motivo		CHAR(300) );");
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			sqlite3_close(db);
			return 0;
	}
	\endverbatim
  */
 /**
  * \fn int insertar_basededatos(char *rutabase_datos, float *humidity, float *temperature, char* tiempo_actual){
  *			\brief Función que se encarga de insertar los datos de humedad, temperatura y la fecha-hora actual de la captura.
  *			@param Los parametros introducidos provienene del programa main.c y del hum_temp.c
  *			@return No devuelve ningún valor.
	\verbatim
			sqlite3 *db;
			int rc;
			char *zErrMsg;
			char sql[256];
			char unidadhumedadrelativa[8] = "rh %";
			char temperatura_unidad[8] = "ºC";

			rc = sqlite3_open(rutabase_datos,&db);
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql,"INSERT INTO Datos_lecturas (ID,Humedad, Unidad, Fecha_hora_lectura)"\
								"VALUES (1,%.2f,'%s','%s');",*humidity, unidadhumedadrelativa, tiempo_actual);
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql,"INSERT INTO Datos_lecturas (ID,Temperatura, Unidad, Fecha_hora_lectura)"\
						"VALUES (2, %.2f,'%s','%s');",*temperature,temperatura_unidad,tiempo_actual);
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

			memset(sql,'\0',sizeof(sql[0]));
			//sprintf(sql,"SELECT * FROM Datos_lecturas;");
			//rc = sqlite3_exec(db, sql, callback_mostrar, (void*)data, &zErrMsg);
			sqlite3_close(db);
			return 0;
	}
	\endverbatim
  */
 /**
  *  \fn int alarm_basededatos(char* rutabase_datos, char* tiempo_anterior){
  *	\brief Función que se encarga de comprobar si ha habido alguna alarma.
  *	Los valores de alarma, serían:
  *		- Valores máximo y mínimos humedad, temperatura.
  *		- Valores inferiores a 0.
  *	\param Los parametros introducidos provienene del programa main.c y del hum_temp.c
  *	\return No devuelve ningún valor.

	\verbatim
			sqlite3 *db;
			int rc;
			char *zErrMsg;
			char sql[256];
			char data[200];
			char *date_alarma;
			char *valor_alarma;
			float values_humedad = 0;
			float values_temperatura = 0;
			char separador[2] = "$";

			rc = sqlite3_open(rutabase_datos,&db);
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql, "SELECT MAX(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
									"WHERE ID=1 AND Fecha_hora_lectura > '%s'",tiempo_anterior);
			rc = sqlite3_exec(db, sql, callback_alarma, (void*)data, &zErrMsg);

			printf("La información que contiene data es %s\n",data);
			valor_alarma = strtok(data, separador);
			date_alarma = strtok(NULL, separador);
			values_humedad = atof(valor_alarma);
			printf("El valor de humedad 1 es %f\n",values_humedad);

			if (values_humedad > 70 && values_humedad <= 100){
				memset(sql,'\0', sizeof(sql[0]));
				sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
							"VALUES ('Se ha realizado una humedad relativa superior al 70 por ciento', '%s');",date_alarma);
				rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			if (values_humedad > 100){
				memset(sql,'\0', sizeof(sql[0]));
				sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
							"VALUES ('Desconfiguración erronea del programa SENSE HAT', '%s');",date_alarma);
				rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql, "SELECT MIN(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
						"WHERE ID=1 AND Fecha_hora_lectura > '%s'",tiempo_anterior);
			rc = sqlite3_exec(db, sql, callback_alarma, (void*)data, &zErrMsg);

			printf("La información que contiene data es %s\n",data);
			valor_alarma = strtok(data, separador);
			date_alarma = strtok(NULL, separador);
			values_humedad = atof(valor_alarma);
			printf("El valor de humedad 2 es %f\n",values_humedad);

			if (values_humedad < 30 && values_humedad >= 0){
					memset(sql,'\0',sizeof(sql[0]));
					sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
							"VALUES('Se ha leido una humedad relativa inferior al 40 por ciento', '%s');", date_alarma);
					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			if (values_humedad < 0){
					memset(sql,'\0',sizeof(sql[0]));
					sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
							"VALUES('No se detecta el sensor de humedad', '%s');", date_alarma);
					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql, "SELECT MAX(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
						 "WHERE ID=2 AND Fecha_hora_lectura > '%s'",tiempo_anterior);
			rc = sqlite3_exec(db, sql, callback_alarma, (void*)data, &zErrMsg);
			printf("La información que contiene data es %s\n",data);
			valor_alarma = strtok(data, separador);
			date_alarma = strtok(NULL, separador);
			values_temperatura = atof(valor_alarma);
			printf("El valor de temperatura 1 es %f\n",values_temperatura);

			if(values_temperatura > 60 && values_temperatura < 70){
				memset(sql,'\0',sizeof(sql[0]));
				sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
							 "VALUES('Se ha leido una temperatura superior entre 60 y 70 grados Centigrados', '%s');",date_alarma);
				rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			if(values_temperatura > 70){
				memset(sql,'\0',sizeof(sql[0]));
				sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
							 "VALUES('Se recomienda desconectar la Raspberry PI, por razones de seguridad', '%s');",date_alarma);
				rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

			}
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql, "SELECT MIN(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
						"WHERE ID=2 AND Fecha_hora_lectura > '%s'",tiempo_anterior);
			rc = sqlite3_exec(db, sql, callback_alarma, data, &zErrMsg);
			printf("La información que contiene data es %s\n",data);
			valor_alarma = strtok(data, separador);
			date_alarma = strtok(NULL, separador);
			values_temperatura = atof(valor_alarma);
			printf("El valor de temperatura 2 es %f\n",values_temperatura);

			if(values_temperatura < 15 ){
				memset(sql,'\0',sizeof(sql[0]));
				sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
							"VALUES('Se ha leido una temperatura inferior a 15 grados Centigrados', '%s');",date_alarma);
				rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			if(values_temperatura < 0 ){
				memset(sql,'\0',sizeof(sql[0]));
				sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
							"VALUES('Se recomienda desconectar la Raspberry PI, por razones de seguridad', '%s');",date_alarma);
				rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
			}
			memset(sql,'\0',sizeof(sql[0]));
			sprintf(sql,"SELECT * FROM Datos_alarm;");
			//rc = sqlite3_exec(db, sql, callback_mostrar, (void*)data, &zErrMsg);
			sqlite3_close(db);
			return 0;
	}
	\endverbatim
  */

/**
 * \fn int callback(void *NotUsed, int argc, char **argv, char **azColName){
 *		\brief Función normalizada para tratar con la base de datos. En este caso se realiza la inserción de datos de la captura.
 *		@param Los parametros introducidos son propios de esta función normalizada.
 *		\return No devuelve ningún valor.
 *
	\verbatim
		   int i;
		   for(i = 0; i<argc; i++) {
			  printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		   }
		   printf("\n");
		   return 0;
	}
	\endverbatim
 */

/**
 * \fn int callback_alarma(void *data, int argc, char **argv, char **azColName){
 *
 *		\brief Función normalizada para tratar con la base de datos. En este caso se realiza la inserción de alarmas.
 *		@param Los parametros introducidos son propios de esta función normalizada.
 *		\return No devuelve ningún valor.
	\verbatim
			printf("\n");
			sprintf(data, "%s$%s", argv[0],argv[1]);
			return 0;
	}
	\endverbatim
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "basededatos.h"

//Crear base de datos
int  crear_basededatos(char *rutabase_datos) {
	sqlite3 *db;
	int rc;
	char *zErrMsg;
	char sql[256];

	rc = sqlite3_open(rutabase_datos, &db);

	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "CREATE TABLE Datos_sensor( " \
				"ID		INT ," \
				"TIPO		CHAR(300) ," \
				"DESCRIPCIÓN	CHAR(300) );");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	memset(sql, '\0', sizeof(sql[0])); 
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg); //escriu al bash
	sprintf(sql, "INSERT INTO Datos_sensor (ID,TIPO,DESCRIPCIÓN) "\
				"VALUES (1, 'Humedad', 'Humedad la placa solar.' );"\
				"INSERT INTO Datos_sensor(ID,TIPO,DESCRIPCIÓN)"\
				"VALUES (2, 'Temperatura', 'Temperatura ambiental');");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	memset(sql, '\0', sizeof(sql[0]));	
	sprintf(sql, "CREATE TABLE Datos_lecturas( " \
				"ID		INT,"\
				"Humedad	FLOAT ,"\
				"Temperatura	FLOAT ,"\
				"Unidad	CHAR(8) ,Fecha_hora_lectura		DATE );");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "CREATE TABLE Datos_alarm( " \
				"Fecha_hora_alarm		DATE,"\
				"Motivo		CHAR(300) );");
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	sqlite3_close(db);
	return 0;
}
int insertar_basededatos(char *rutabase_datos, float *humidity, float *temperature, char* tiempo_actual) {

		sqlite3 *db;
		int rc;
		char *zErrMsg;
		char sql[256];
		char unidadhumedadrelativa[8] = "rh %";
		char temperatura_unidad[8] = "ºC";

		rc = sqlite3_open(rutabase_datos, &db);
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_lecturas (ID,Humedad, Unidad, Fecha_hora_lectura)"\
					"VALUES (1,%.2f,'%s','%s');", *humidity, unidadhumedadrelativa, tiempo_actual); 			
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_lecturas (ID,Temperatura, Unidad, Fecha_hora_lectura)"\
					"VALUES (2, %.2f,'%s','%s');", *temperature, temperatura_unidad, tiempo_actual); 			
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

		memset(sql, '\0', sizeof(sql[0]));
	sqlite3_close(db);
	return 0;
}

int alarm_basededatos(char *rutabase_datos, char *tiempo_anterior) {

	sqlite3 *db;
	int rc;
	char *zErrMsg;
	char sql[256];
	char data[200];
	char *date_alarma;
	char *valor_alarma;
	float values_humedad = 0;
	float values_temperatura = 0;
	char separador[2] = "$";

	rc = sqlite3_open(rutabase_datos, &db);
	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "SELECT MAX(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
							"WHERE ID=1 AND Fecha_hora_lectura > '%s'", tiempo_anterior);
	rc = sqlite3_exec(db, sql, callback_alarma, (void *)data, &zErrMsg);

	printf("La información que contiene data es %s\n", data);
	valor_alarma = strtok(data, separador);
	date_alarma = strtok(NULL, separador);
	values_humedad = atof(valor_alarma);
	printf("El valor de humedad 1 es %f\n", values_humedad);

	if (values_humedad > 70 && values_humedad <= 100) {
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
					"VALUES ('Se ha realizado una humedad relativa superior al 70 por ciento', '%s');", date_alarma);
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	if (values_humedad > 100) {
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
					"VALUES ('Desconfiguración erronea del programa SENSE HAT', '%s');", date_alarma);
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "SELECT MIN(Humedad),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID=1 AND Fecha_hora_lectura > '%s'", tiempo_anterior);
	rc = sqlite3_exec(db, sql, callback_alarma, (void *)data, &zErrMsg);

	printf("La información que contiene data es %s\n", data);
	valor_alarma = strtok(data, separador);
	date_alarma = strtok(NULL, separador);
	values_humedad = atof(valor_alarma);
	printf("El valor de humedad 2 es %f\n", values_humedad);

	if (values_humedad < 30 && values_humedad >= 0) {
			memset(sql, '\0', sizeof(sql[0]));
			sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
					"VALUES('Se ha leido una humedad relativa inferior al 40 por ciento', '%s');", date_alarma);
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	if (values_humedad < 0) {
			memset(sql, '\0', sizeof(sql[0]));
			sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
					"VALUES('No se detecta el sensor de humedad', '%s');", date_alarma);
			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "SELECT MAX(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
				 "WHERE ID=2 AND Fecha_hora_lectura > '%s'", tiempo_anterior);
	rc = sqlite3_exec(db, sql, callback_alarma, (void *)data, &zErrMsg);
	printf("La información que contiene data es %s\n", data);
	valor_alarma = strtok(data, separador);
	date_alarma = strtok(NULL, separador);
	values_temperatura = atof(valor_alarma);
	printf("El valor de temperatura 1 es %f\n", values_temperatura);

	if (values_temperatura > 60 && values_temperatura < 70) {
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
					 "VALUES('Se ha leido una temperatura superior entre 60 y 70 grados Centigrados', '%s');", date_alarma);
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	if (values_temperatura > 70) {
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_alarm (Motivo,Fecha_hora_alarm)"\
					 "VALUES('Se recomienda desconectar la Raspberry PI, por razones de seguridad', '%s');", date_alarma);
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	}
	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "SELECT MIN(Temperatura),Fecha_hora_lectura FROM Datos_lecturas "\
				"WHERE ID=2 AND Fecha_hora_lectura > '%s'", tiempo_anterior);
	rc = sqlite3_exec(db, sql, callback_alarma, data, &zErrMsg);
	printf("La información que contiene data es %s\n", data);
	valor_alarma = strtok(data, separador);
	date_alarma = strtok(NULL, separador);
	values_temperatura = atof(valor_alarma);
	printf("El valor de temperatura 2 es %f\n", values_temperatura);

	if (values_temperatura < 15) {
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
					"VALUES('Se ha leido una temperatura inferior a 15 grados Centigrados', '%s');", date_alarma);
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	if (values_temperatura < 0) {
		memset(sql, '\0', sizeof(sql[0]));
		sprintf(sql, "INSERT INTO Datos_alarm (Motivo, Fecha_hora_alarm)"\
					"VALUES('Se recomienda desconectar la Raspberry PI, por razones de seguridad', '%s');", date_alarma);
		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	}
	memset(sql, '\0', sizeof(sql[0]));
	sprintf(sql, "SELECT * FROM Datos_alarm;");
	//rc = sqlite3_exec(db, sql, callback_mostrar, (void*)data, &zErrMsg);
	sqlite3_close(db);
	return 0;
}


int callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;

   for (i = 0; i < argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int callback_alarma(void *data, int argc, char **argv, char **azColName) {

	printf("\n");
	sprintf(data, "%s$%s", argv[0], argv[1]);
	return 0;
}

