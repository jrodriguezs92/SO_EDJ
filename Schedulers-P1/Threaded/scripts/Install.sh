#!/bin/bash

# Author: Jeremy Rodriguez
#         Daniela Hernandez
#         Esteban Agüero
# Date: 02/03/2019

# Parameter: Port number to be mapped for the container
############################################################


echo -e '\e[33m> Building Docker Image \e[39m'

# Go to the root directory of the web server.
cd ..
# Build image from Dockerfile.
sudo docker build --tag=webserver_threaded .

echo -e '\e[33m> Executing Docker container \e[39m'

# Run Docker container: 
# in detached mode, with parameter as mapped port, 
# in privileged mode (for the use of systemctl commands), 
# with friendly name "webserver", 
# and with a defined volume.
# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
sudo docker run -d -p 8005:8005 --privileged --name webserver_threaded -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_threaded

echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

# Execute command inside the container in detached mode, to install Daemon and run it.
sudo docker exec -d webserver_threaded ./scripts/InstallDaemon.sh

echo -e '\e[33m> successfully completed! \e[39m'