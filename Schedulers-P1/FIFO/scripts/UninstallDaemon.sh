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
systemctl stop webserver.service
systemctl disable webserver.service
rm /etc/systemd/system/webserver.service
rm -f /usr/bin/WebServer
#sudo rm -f /usr/lib/systemd/system/webserver.service
rm -rf /etc/webserver/
rm -f /etc/webserver/webserver.conf
rm -f /var/log/webserver.log
systemctl daemon-reload
systemctl reset-failed
echo "> Completed"
