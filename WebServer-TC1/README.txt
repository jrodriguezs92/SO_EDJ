***********************************************************

	Instituto Tecnológico de Costa Rica
	Computer Engineering

	Programmer: Esteban Agüero Pérez (estape11)

	Last update: 26/02/2019

	Operating Systems Principles
	Professor. Diego Vargas

***********************************************************

> To compile, in the root folder run:
	make all

> If you like to compile only the lib
	cd lib
	make all / make libserver.a

> To execute:
	./WebServer -flags
		> Where flags:
			-h --help                 Print help
			-c --conf_file filename   Read configuration from the file
			-t --test_conf filename   Test configuration file
			-l --log_file  filename   Write logs to the file
			-d --daemon               Daemonize this application
			-p --pid_file  filename   PID file used by daemonized app

	# Example:
		estape11@MacBook:~/WebServer-TC1$ ./bin/WebServer
		> Server started
	 		Port: 10101 
	 		Root directory: ~/home/user/res

> To remove compilation files
	make clean

> To install the daemon
		./scripts/InstallDaemon

> To uninstall the daemon
		./scripts/UninstallDaemon
		
* To use more content, copy the files in bin/res
* To generate file file.ext of X bytes
	dd if=/dev/zero of=file.ext bs=1 count=0 seek=X
* Can be X=1G, o 1k
