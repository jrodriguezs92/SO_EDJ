#! /bin/bash

#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 26/02/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

echo "> Installing WebServer Daemon"
sudo cp ../bin/WebServer /usr/bin/
sudo cp webserver.service /etc/systemd/system/
sudo mkdir -p /etc/webserver/
sudo cp webserver.conf /etc/webserver/
sudo systemctl daemon-reload
echo "> Completed"
echo "Start daemon: sudo systemctl start webserver.service"
echo "Status daemon: systemctl status webserver.service"
