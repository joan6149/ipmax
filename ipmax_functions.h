
/*File ipmax_functions.h*/

#ifndef IPMAX_FUNCTIONS_H
#define IPMAX_FUNCTIONS_H
/*release path*/
//#define PATH "/etc/ipmax/ips.list"
/*Debug path*/
#define PATH "/home/joan/ips.list"
#define BBDD "/home/joan/ipmax.db"


struct bodyMail {
	char* ip;
	char* open_ports;
};


int comparaIp(char* ip);
char* readLastIp(char* fp);
char* openPorts(); //Pendiente implementacion
char* bodyMail(); //Pendiente bodyMail
int insertIp(char* ip);
int createTable();
int consultaUltimaIp(char* ip);
int listAllIps(void *nada, int argc, char** argv, char** columns); 
int allIps();


#endif
