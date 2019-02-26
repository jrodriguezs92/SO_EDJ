/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Ingeniería en Computadores

		Programador: Esteban Agüero Pérez (estape11)
		Lenguaje: C++
		Versión: 1.0
		Última Modificación: 24/02/2019

		Entradas: Valor de puerto y/o directorio root
		Restricciones: Puerto numerico / Directorio string

					Principios de Sistemas Operativos
						Prof. Diego Vargas

*******************************************************************
*/

#include <server.h>

int main(int argc, char* argv[]){

	struct sockaddr_in clienteAddr;
	socklen_t addrLen;
	char bandera;    
	
	// el directorio por defecto = ~/res y puerto puerto=10101
	char puerto[6];
	dirRoot = getenv("PWD"); // obtiene la variable del entorno del directorio actual
	strcat(dirRoot, "/res"); // $(PWD)/res
	strcpy(puerto,"10101");

	int slot=0;
	int	last_fd;

	//Parseo de los argumentos
	while ((bandera = getopt (argc, argv, "p:r:")) != -1)
		switch (bandera) {
			case 'r':
				dirRoot = malloc(strlen(optarg));
				strcpy(dirRoot,optarg);
				break;
			case 'p':
				strcpy(puerto,optarg);
				break;
			case '?':
				fprintf(stderr,"Argumento invalido\n");
				exit(1);
			default:
				exit(1);
		}
	
	printf("> Servidor iniciado\n\t Puerto: %s%s%s \n\t Directorio Root: %s%s%s\n","\033[92m",puerto,"\033[0m","\033[92m",dirRoot,"\033[0m");

	// se ponen todos los clientes en -1, que significa que no esta conectado
	int i;
	for (i=0; i<CONEXMAX; i++) {
		clientes[i]=-1;

	}
	iniciarServidor(puerto);

	// loop para aceptar conexiones nuevas
	while (1) {
		last_fd = sockfd;
		addrLen = sizeof(clienteAddr);
		clientes[slot] = accept (sockfd, (struct sockaddr *) &clienteAddr, &addrLen); // system call para crer la conexion de sockets

		if (clientes[slot]<0){
			perror("accept() error");

		}

		else {
			responderSolicitud(slot); // atiende una solicitud a la vez
			/*
			// codigo para aceptar mas de una solicitud a la vez
			if ( fork()==0 ){
				responderSolicitud(slot);
				exit(0);

			}
			*/
		}

		while (clientes[slot]!=-1) {
			slot = (slot+1)%CONEXMAX; 

		}
	}
	
	return 0;
}