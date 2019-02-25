/*server.c*/

/*
*******************************************************************

				Instituto Tecnológico de Costa Rica
					Ingeniería en Computadores

		Programador: Esteban Agüero Pérez (estape11)
		Lenguaje: C++
		Versión: 1.0
		Última Modificación: 24/02/2019

					Principios de Sistemas Operativos
						Prof. Diego Vargas

*******************************************************************
*/

#include <server.h>

void iniciarServidor(char* puerto) {
	struct addrinfo hints, *res, *p;

	// getaddrinfo para el host
	memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo( NULL, puerto, &hints, &res) != 0) {
		perror ("getaddrinfo() error");
		exit(1);

	}

	// socket y bind
	for (p = res; p!=NULL; p=p->ai_next){
		sockfd = socket (p->ai_family, p->ai_socktype, 0); // se crea el punto de conexion para conexion

		if (sockfd == -1) {
			continue;

		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) { // se asigna la direccion al socket
			break;

		}

	}
	if (p==NULL) { // en caso que no se pueda crear la conexion
		perror ("socket() or bind()");
		exit(1);

	}

	freeaddrinfo(res); // se libera la la memoria dinamica de res, para evitar leaks de memoria

	// se escucha a las conexiones entrantes
	if ( listen (sockfd, 1000000) != 0 ) {
		perror("listen() error");
		exit(1);

	}
}

void responderSolicitud(int n) {
	printf("** Inicio comunicacion con %i **\n",n);
	char mensaje[MSGLEN], *reqline[3], data_to_send[BYTES], path[MSGLEN];
	int rcvd, fd, bytesLeidos;
	memset( (void*) mensaje, (int)'\0', MSGLEN );
	rcvd=recv(clientes[n], mensaje, MSGLEN, 0);

	if (rcvd<0) {    // se recibe error
		fprintf(stderr,("recv() error\n"));

	}

	else if (rcvd==0) {    // socket recibido cerrado
		fprintf(stderr,"> Cliente desconectado.\n");

	}

	else if((strcmp(mensaje, "\n")) != 0){    // mensaje recibido
		printf("Mensaje recibido: \n%s", mensaje);
		reqline[0] = strtok (mensaje, " \t\n");

		if ( strncmp(reqline[0], "GET\0", 4)==0 ){
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.1", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )	{
				write(clientes[n], "HTTP/1.1 400 Bad Request\n", 25);

			}

			else {
				if ( strncmp(reqline[1], "/\0", 2)==0 ) {
					reqline[1] = "/index.html"; // pagina por defecto
				}

				strcpy(path, dirRoot);
				strcpy(&path[strlen(dirRoot)], reqline[1]);
				printf("Enviando: %s\n", path);

				if ( (fd=open(path, O_RDONLY))!=-1 ) { // archivo encontrado
					send(clientes[n], "HTTP/1.1 200 OK\n\n", 17, 0);

					while ( (bytesLeidos=read(fd, data_to_send, BYTES))>0 ) {
						write (clientes[n], data_to_send, bytesLeidos);

					}

				}
				else { // archivo no encontrado
					write(clientes[n], "HTTP/1.1 404 Not Found\n", 23);

				}
			}
		}
	}

	// cerrado del socket
	shutdown (clientes[n], SHUT_RDWR); // apaga la conexion
	close(clientes[n]);
	clientes[n]=-1;
	printf("** Fin comunicacion con %i **\n",n);
}

/*server.c*/