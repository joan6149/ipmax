#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include "ipmax_functions.h"
#include<ipify.h>

#define URL "https://api.ipify.org?format=text"
//#define PATH "/etc/ipmax/ips.list"

int main(int argc, char* argv[]) {
	
	if(argc > 1 && strcmp(argv[1], "-list") == 0) {
		allIps();
		exit(0);
	}

	int fd2[2];
	//int file;
	//char* ip;
	int pid;
	pipe(fd2);
	if((pid=fork()) == 0) {
		char cad[256];
		char* mail;
		//close(1);
		//dup(fd2[1]);
		close(fd2[0]);
		//close(fd2[1]);
		ipify(cad, sizeof(cad));
		printf("cad is %s\n",cad);
		if(consultaUltimaIp(cad) == 0) { //la nueva version tiene que consultarla de la BBDD
			//write(fd2[1], "0", 1);
			close(fd2[1]);
			exit(0);
		}else {
			if(insertIp(cad) < 0) {
				fprintf(stderr ,"error al insertar\n");
				exit(2);
			}
			mail = (char*)malloc(sizeof(cad)+256);
			sprintf(mail, "Tu ip actual es: %s", cad);
			printf("%s\n", mail);
			//Obtenemos los servicios de mi ip
			write(fd2[1], mail, strlen(mail));
			close(fd2[1]);
			exit(0);
		}
	}
	if(fork() == 0) {
		close(0);
		dup(fd2[0]);
		close(fd2[1]);
		close(fd2[0]);
		char* recv_mail;
		char* command;
		const char* email = "joan6149@gmail.com";
		recv_mail = (char*)malloc(1024);
		command = (char*)malloc(sizeof(recv_mail)+1024);
		if(read(0, recv_mail, 1024) == 0) {
			printf("La ips eran iguales i por lo tanto no se envia nada.\n");
		} else {
			sprintf(command, "mail -s \"%s\" %s", recv_mail, email);
			system(command);
			//execlp("mail", "-s", recv_mail ,"joan6149@gmail.com", NULL);
			exit(2);
		}
	}

	close(fd2[0]);
	close(fd2[1]);
	wait(NULL);
	wait(NULL);
	exit(0);


}


