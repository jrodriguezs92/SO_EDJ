#!/bin/bash

# Author: Jeremy Rodriguez
#         Daniela Hernandez
#         Esteban Agüero
# Date: 02/03/2019
############################################################

# Script absolute path
SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`


################# FIFO SERVER

echo -e '\e[44m>>> INSTALLING FIFO SERVER \e[49m'

echo -e '\e[33m> Building Docker Image \e[39m'

# Build image from Dockerfile.
sudo docker build --tag=webserver_fifo $SCRIPTPATH/FIFO/

echo -e '\e[33m> Executing Docker container \e[39m'

# Run Docker container: 
# in detached mode,
# in privileged mode (for the use of systemctl commands), 
# with friendly name "webserver", 
# and with a defined volume.
# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
sudo docker run -d -p 8001:8001 --privileged --name webserver_fifo -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_fifo

echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

# Execute command inside the container in detached mode, to install Daemon and run it.
sudo docker exec -d webserver_fifo ./scripts/InstallDaemon.sh

echo -e '\e[33m> successfully completed! \e[39m'



################# FORKED SERVER

echo -e '\e[44m>>> INSTALLING FORKED SERVER \e[49m'

echo -e '\e[33m> Building Docker Image \e[39m'

# Build image from Dockerfile.
sudo docker build --tag=webserver_forked $SCRIPTPATH/Forked/

echo -e '\e[33m> Executing Docker container \e[39m'

# Run Docker container: 
# in detached mode,
# in privileged mode (for the use of systemctl commands), 
# with friendly name "webserver", 
# and with a defined volume.
# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
sudo docker run -d -p 8003:8003 --privileged --name webserver_forked -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_forked

echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

# Execute command inside the container in detached mode, to install Daemon and run it.
sudo docker exec -d webserver_forked ./scripts/InstallDaemon.sh

echo -e '\e[33m> successfully completed! \e[39m'



################# THREADED SERVER

echo -e '\e[44m>>> INSTALLING THREADED SERVER \e[49m'

echo -e '\e[33m> Building Docker Image \e[39m'

# Build image from Dockerfile.
sudo docker build --tag=webserver_threaded $SCRIPTPATH/Threaded/

echo -e '\e[33m> Executing Docker container \e[39m'

# Run Docker container: 
# in detached mode,
# in privileged mode (for the use of systemctl commands), 
# with friendly name "webserver", 
# and with a defined volume.
# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
sudo docker run -d -p 8005:8005 --privileged --name webserver_threaded -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_threaded

echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

# Execute command inside the container in detached mode, to install Daemon and run it.
sudo docker exec -d webserver_threaded ./scripts/InstallDaemon.sh

echo -e '\e[33m> successfully completed! \e[39m'