# SO, Project 1
Instituto Tecnológico de Costa Rica  
Computer Engineering (CE).  
Project 1: Operating System Principles.  
2019, I Semester.    

## Environment
GNU/Linux Ubuntu 16.04.

## Instructions for the scripts
### Install
1. Execute script "Install" that is located in the root of the project directory:
```
./Install.sh p1 p2 p3 p4 p5
```
This will: install Docker Community Edition in the system, install the five docker containers with the 5 ports (p#) to map as rguments, run all the containers and install and run the server daemon inside each container.

Example:

./Install.sh 8001 8003 8005 8007 8009

Also, you can type './Install.sh help', to show the help commands.
### Uninstall
1. Execute script "Uninstall" that is located in the root of the project directory:
```
./Uninstall.sh
```
This will: stop and remove each of the five docker container and remove each of the five main docker image.
##  Instructions for execution
If the container is successfully executed and is running, we enter to the container with:
```
sudo docker exec -it webserver_x bash
```
Where x is the version you want it to run (fifo, forked, threaded ...).

The resource folder for each web server is in the path: "/usr/src/ws/bin/res" inside each container. Everything you want to request from the server need to be in this folder.

To generate file file.ext of X bytes "dd if=/dev/zero of=file.ext bs=1 count=0 seek=X".

Where, the "X" in "seek" argument means the file size. E.g.: ...seek=2G or ...seek=100M.

## Instructions for manually install and uninstall
The "Install.sh" and "Uninstall.sh" scripts already take care of the end-to-end process, but this section explain the manually process in case you want to learn about the low level functionality.

Note that all the Docker commands begin with "sudo".  

1. First, we need to build each Dockerfile included into each server folder, with a representative '--tag' as option (be sure you use a different tag for each build server):
```
sudo docker build --tag=webserver .
```
2. We can see all the images created from this build with:
```
sudo docker images
```
1. Create and execute the Docker container, with a representative '--name' as option (be sure you use a different name for each run container) and the image tag as a parameter. Also, you need to map the port to be used with '-p' option, each container have a different port exposed (FIFO->8001, FORKED->8003, THREADED->8005, PREFORKED->8007, PRETHREADED->8009). The form is 'LocalPort:ContainerPort':
```
sudo docker run -d -p 8001:8001 --privileged --name webserver -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver
```
4. We can see all the running containers with:
```
sudo docker ps
```
5. We can see all the containers (include the stopped containers) with:
```
sudo docker ps -a
```
6. If the container is successfully executed and is running, we enter to the container with:
```
sudo docker exec -it webserver_x bash
```
7. Next, we need to install the web server application, we use the current commands for Linux without any "sudo" instruction:
```
cd scripts
```
```
./InstallDaemon.sh
```
8. To run the daemon, execute the command:
```
systemctl start webserver.service
```
9. To view the process status:
```
systemctl status webserver.service
```
10. To uninstall the web server:
```
./UninstallDaemon
```
11. To exit of the container bash:
```
exit
```
12. To stop the container:
```
sudo docker stop webserver
```
13. To delete the container:
```
sudo docker rm <CONTAINER ID>
```
you can see the CONTAINDER ID with:
```
sudo docker ps
```
14. To delete the image:
```
sudo docker rmi <IMAGE ID>
```
you can see the IMAGE ID with:
```
sudo docker images
```

## Autor
* Jeremy Rodríguez Solórzano  
jrodriguezs0292@gmail.com
* Esteban Agüero Pérez  
estape11@gmail.com
* Daniela Hernández Alvarado  
dannyha07@gmail.com

## Version
1.0.0