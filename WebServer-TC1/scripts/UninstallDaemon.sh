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

echo "> Uninstalling WebServer Daemon"
sudo systemctl stop webserver.service
sudo systemctl disable webserver.service
sudo rm -f /usr/bin/WebServer
sudo rm -f /usr/lib/systemd/system/webserver.service
sudo rm -rf /etc/webserver/
sudo rm -f /etc/webserver/webserver.conf
sudo rm -f /var/log/webserver.log
sudo systemctl daemon-reload
sudo systemctl reset-failed
echo "> Completed"
