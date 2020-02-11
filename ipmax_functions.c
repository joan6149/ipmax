
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<sqlite3.h>
#include<time.h>
#include "ipmax_functions.h"

//#define PATH "/etc/ipmax/ips.list"

int comparaIp(char *ip) {
	FILE* fp;
	char *last_ip;
	last_ip = malloc(sizeof(char)*17);
	int return_value = 0;
	fp = fopen(PATH, "r");
	if(fp == NULL) {
		fp = fopen(PATH, "w");
		//printf("Estoy dentro del W\n");
		fwrite(ip, sizeof(char), strlen(ip), fp);
		fclose(fp);
	} else {
		fclose(fp);
		last_ip = readLastIp(PATH);
		//printf("last_ip: %s ; ip: %s\n", last_ip, ip);
		if(strcmp(last_ip, ip) == 0) { //me faltra la comparacion y la funcion 
			return(return_value);
		} else {
			return_value = 1;
		}
	}
	return(return_value);
}

char* readLastIp(char* ipsFile) {
	FILE* fp;
	char *last_line;
	char *last_ip;
	char *aux;
	aux = malloc(sizeof(char)*17);
	if((fp = fopen(ipsFile, "r")) != NULL) {
		fseek(fp, -(sizeof(char)*17), SEEK_END);
		fread(aux, sizeof(char), 17, fp);
		last_line = strchr(aux, '\n');
		fclose(fp);
		last_ip = last_line + 1;
		last_ip[strlen(last_ip)-1] = '\0';
		//printf("%s", last_ip);

	}
	return last_ip;
}

int insertIp(char* ip) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	time_t now = time(NULL);
	struct tm* localnow = localtime(&now);
	char time[100];
	int res;
	char* insert_query;
	char* err;
	int err_code = 0;
	insert_query = malloc(sizeof(char)*1024);

	if((res = sqlite3_open(BBDD, &db)) != SQLITE_OK) {
		printf("Error al abrir BBDD\n");
	}
	strftime(time, 100, "%d/%m/%Y", localnow);
	sprintf(insert_query, "INSERT INTO IPS VALUES ('%s', '%s');", ip, time);
	printf("%s\n",insert_query);
	/******************NEW PART********************************************/
	if(sqlite3_prepare(db, insert_query, strlen(insert_query), &stmt, 0) != SQLITE_OK) {
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		printf("Error preparando statement\n");
		printf("Intentando crear tabla...\n");
	}
	if((err_code = sqlite3_step(stmt)) != SQLITE_DONE) {
		printf("Error al insertar con STMT: %s\n", err_code);
		exit(2);
	}
	/**********************************************************************/
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}

int createTable() {
	int res;
	sqlite3* db;
	char* create_table;
	res = sqlite3_open(BBDD, &db);
	create_table = "CREATE TABLE IPS (Ip TEXT, Cuando DATETIME);";
	if((res = sqlite3_exec(db, create_table, NULL, 0, NULL)) != SQLITE_OK) {
		sqlite3_close(db);
		return(-1);
	}
	sqlite3_close(db);
	return(0);
}

int listAllIps(void *nada, int argc, char** argv, char** columns) {
	int i;

	for(i=0;i<argc;i++) {
		printf("%s -> %s\n",columns[i], argv[i]);
	}
	printf("\n");
	return(0);	

}

int allIps() {  //OJO SOLO CONSULTA LA ULTIMA INSERCION DEVUELVE 0 SI LA IP QUE LE PASAMOS ES LA MISMA QUE LA ULTIMA IP INSERTADA
	int res = 0;
	sqlite3* db;
	char* query;
	query = malloc(sizeof(char)*1024);
	res = sqlite3_open(BBDD, &db);
	sprintf(query, "SELECT * FROM IPS ORDER BY Cuando;");
	if((res=sqlite3_exec(db, query, listAllIps, 0, NULL)) != SQLITE_OK) { //hay que mirar si esto piodria ser una consulta
		sqlite3_close(db);
		if((res=createTable()) < 0) {
			printf("Error al crear tabla\n");
			return(-1);
		} else {
			printf("Se crea la tabla y se vuelve a cinsultar la op\n");
			allIps();
		}
	}

	sqlite3_close(db);

	return(0); 
}

int consultaUltimaIp(char* ip) {  //OJO SOLO CONSULTA LA ULTIMA INSERCION DEVUELVE 0 SI LA IP QUE LE PASAMOS ES LA MISMA QUE LA ULTIMA IP INSERTADA
	int res = 0;
	int iguales = 0;
	sqlite3* db;
	sqlite3_stmt *stmt;
	char* query;
	query = malloc(sizeof(char)*1024);
	res = sqlite3_open(BBDD, &db);
	if(res != SQLITE_OK) {
		printf("Error al abrir la bbdd\n");
	}
	sprintf(query, "SELECT Ip FROM IPS ORDER BY Cuando DESC LIMIT 1;");
	if(sqlite3_prepare(db, query, strlen(query), &stmt, 0) != SQLITE_OK) {
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		printf("Error preparando statement\n");
		printf("Intentando crear tabla...\n");
		if((res=createTable()) < 0) {
			printf("Error al crear tabla, puede que la tabla ya exista...\n");
			return(-1);
		} else {
			printf("Se crea la tabla y se vuelve a cinsultar la op\n");
			sqlite3_close(db);
			consultaUltimaIp(ip);
		}
	} else {
		if(sqlite3_step(stmt) == SQLITE_ROW) {
			char* obtenida = sqlite3_column_text(stmt, 0);
			printf("%s\n", obtenida); //hay que poner que coja la ultima y no la primera intriducida
			if(strcmp(obtenida, ip) == 0) {
				printf("Tu ip actual es la misma que la guardada, no se hace nada\n");
			} else {
				printf("Tu ip actual NO es la misma que la guardada, SE ENVIA EL MAIL!!\n");
				iguales = 1;
			}
		} else {
			insertIp(ip);
		}

	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return(iguales); //mira alguna manera de poder hacer que esta variable sea global
}
