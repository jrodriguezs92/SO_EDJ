************************************************************

	Instituto Tecnológico de Costa Rica
	Ingeniería en Computadores

	Programador: Esteban Agüero Pérez (estape11)

	Última Modificación: 24/02/2019

	Principios de Sistemas Operativos
	Prof. Diego Vargas

************************************************************

> Para la compilacion, estando en la carpeta raiz:
	make all

> Si se desea solo compilar la libreria server
	cd lib
	make all / make libserver.a

> Pasos para ejecutar:
	./WebServer *-p *XXXX *-r */XX/XX
		> Donde :
			-p : puerto
			-r : directorio root
			(*=opcional)

	# Ejemplo:
		estape11@MacBook:~/WebServer-TC1$ ./bin/WebServer
		> Servidor iniciado
	 		Puerto: 10101 
	 		Directorio Root: ~/WebServer-TC1/bin/res

> Si se desea eliminar todos los archivos de compilacion
	make clean

* Si se desea agregar mas contenido en el servidor, se debe copiar en la carpeta bin/res
* Si se desea generar un archivo de prueba de X cantidad de bytes
	dd if=/dev/zero of=1g.img bs=1 count=0 seek=X
* Puede ser X=1G, o 1k
