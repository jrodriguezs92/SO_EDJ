#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h> // Implementacion del socket
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

// Puerto a usar
#define PORT 8001

// Pagina web a mostrar
char webpage[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>ShellWaveX</title>\r\n"
"<style>body { background-color: #FFFF00 }</style></head>\r\n"
"<body><center><h1>Hello world!</h1><br>\r\n"
"<img src=\"doctest.jpeg\"></center></body></html>\r\n";

int main(int argc, char argv[])
{
    // Variables necesarias para la comunicacion
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;
    char buf[2048];
    int fdimg;
    int on = 1;

    // Inicializar socket de servidor con protocolo TCP/IP
    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    if(fd_server < 0)
    {
        perror("Error al crear Socket");
        exit(1);
    }

    // Descripcion del socket
    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    
    // Descripcion e inicializacion de estructura del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // BIND, llamada al sistema permite enlazar el programa a un puerto proporcionado por el SO
    if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("Error de enlace (Bind)");
        close(fd_server);
        exit(1);
    }

    // Escuchar el puerto determinado para comunicacion
    if(listen(fd_server, SOMAXCONN) == -1)
    {
        perror("Error de comunicacion con puerto "+PORT);
        close(fd_server);
        exit(1);
    }

    while(1)
    {
        // Aceptar comunicacion en socket
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

        if(fd_client == -1)
        {
            perror("Conexion fallida\n");
            continue;
        }

        printf("Conexion en puerto %i\n",PORT);

        if(!fork())
        {
            // Proceso hijo
            close(fd_server);
            memset(buf, 0, 2048);
            read(fd_client, buf, 2047);

            printf("%s\n",buf);

            if(!strncmp(buf, "GET /favicon.ico", 16))
            {
                fdimg = open("favicon.ico", O_RDONLY);
                sendfile(fd_client, fdimg, NULL, 54000);
                close(fdimg);
            }
            else if(!strncmp(buf, "GET /doctest.jpeg", 16))
            {
                fdimg = open("doctest.jpeg", O_RDONLY);
                sendfile(fd_client, fdimg, NULL, 9000);
                close(fdimg);
            }
            else
            {
                write(fd_client, webpage, sizeof(webpage)-1);
            }

            close(fd_client);
            printf("closing...\n");
            exit(0);
        }
        // Proceso padre
        close(fd_client);
    }
    

    return 0;
}