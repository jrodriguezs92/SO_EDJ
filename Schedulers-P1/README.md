# SO, Project 1
Instituto Tecnológico de Costa Rica  
Computer Engineering (CE).  
Project 1: Principios de sistemas operativos.  
2019, I Semester.  

## Prerequisites
Is important to install Docker CE, 18.06.1-ce or later as recommended version.  

## Environment
GNU/Linux Ubuntu 16.04.

## Installing dependencies for Docker Comunity Edition.

To install Docker instalar Docker Comunity Edition you can follow the instructions in: https://docs.docker.com/install/linux/docker-ce/ubuntu/

## Instructions
Note that all the Docker commands begin with "sudo".  

1. First, we need to build the Dockerfile included into the webserver folder:
```
sudo docker build --tag=webserver .
```
2. We can see all the images created from this build with:
```
sudo docker images
```
3. Create and execute the Docker container:
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
sudo docker exec -it webserver bash
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
201209131  
jrodriguezs0292@gmail.com

## Version
1.0.0

## References
https://docs.docker.com/get-started/
