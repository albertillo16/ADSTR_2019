/*
 * basededatos.c
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
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "basededatos.h"

//crear
int  crear_basededatos(char* rutabase_datos){
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
int insertar_basededatos(char *rutabase_datos, float *humidity, float *temperature, char* tiempo_actual){
	
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

int alarm_basededatos(char* rutabase_datos, char* tiempo_anterior){
		
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


int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int callback_delete(void *data, int argc, char **argv, char **azColName){
	
	int i = 0;
	for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
	return 0;
}

int callback_alarma(void *data, int argc, char **argv, char **azColName){

	printf("\n");
	sprintf(data, "%s$%s", argv[0],argv[1]);
	return 0;
}

int callback_mostrar(void *data, int argc, char **argv, char **azColName){
	int i = 0;
	for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
	return 0;
}
