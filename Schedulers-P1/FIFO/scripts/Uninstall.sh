#!/bin/bash

# Author: Jeremy Rodriguez
#         Daniela Hernandez
#         Esteban Agüero
# Date: 02/03/2019

############################################################

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