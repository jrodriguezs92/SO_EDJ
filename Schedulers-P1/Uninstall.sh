#!/bin/bash

# Author: Jeremy Rodriguez
#         Daniela Hernandez
#         Esteban Agüero
# Date: 02/03/2019

############################################################

################# FIFO SERVER

echo -e '\e[44m>>> UNINSTALLING FIFO SERVER \e[49m'

echo -e '\e[33m> Stoping Docker Container \e[39m'

# Stop the container
sudo docker stop webserver_fifo

echo -e '\e[33m> Removing Docker Container \e[39m'

# Remove the container
sudo docker rm webserver_fifo

echo -e '\e[33m> Removing Docker Image \e[39m'

# Remove the image
sudo docker rmi webserver_fifo
    
echo -e '\e[33m> successfully completed! \e[39m'



################# FORKED SERVER

echo -e '\e[44m>>> UNINSTALLING FORKED SERVER \e[49m'

echo -e '\e[33m> Stoping Docker Container \e[39m'

# Stop the container
sudo docker stop webserver_forked

echo -e '\e[33m> Removing Docker Container \e[39m'

# Remove the container
sudo docker rm webserver_forked

echo -e '\e[33m> Removing Docker Image \e[39m'

# Remove the image
sudo docker rmi webserver_forked
    
echo -e '\e[33m> successfully completed! \e[39m'



################# THREADED SERVER

echo -e '\e[44m>>> UNINSTALLING THREADED SERVER \e[49m'

echo -e '\e[33m> Stoping Docker Container \e[39m'

# Stop the container
sudo docker stop webserver_threaded

echo -e '\e[33m> Removing Docker Container \e[39m'

# Remove the container
sudo docker rm webserver_threaded

echo -e '\e[33m> Removing Docker Image \e[39m'

# Remove the image
sudo docker rmi webserver_threaded
    
echo -e '\e[33m> successfully completed! \e[39m'

################# PRE-FORKED SERVER

echo -e '\e[44m>>> UNINSTALLING PRE-FORKED SERVER \e[49m'

echo -e '\e[33m> Stoping Docker Container \e[39m'

# Stop the container
sudo docker stop webserver_preforked

echo -e '\e[33m> Removing Docker Container \e[39m'

# Remove the container
sudo docker rm webserver_preforked

echo -e '\e[33m> Removing Docker Image \e[39m'

# Remove the image
sudo docker rmi webserver_preforked
    
echo -e '\e[33m> successfully completed! \e[39m'

################# PRE-THREADED SERVER

echo -e '\e[44m>>> UNINSTALLING PRE-THREADED SERVER \e[49m'

echo -e '\e[33m> Stoping Docker Container \e[39m'

# Stop the container
sudo docker stop webserver_prethreaded

echo -e '\e[33m> Removing Docker Container \e[39m'

# Remove the container
sudo docker rm webserver_prethreaded

echo -e '\e[33m> Removing Docker Image \e[39m'

# Remove the image
sudo docker rmi webserver_prethreaded
    
echo -e '\e[33m> successfully completed! \e[39m'