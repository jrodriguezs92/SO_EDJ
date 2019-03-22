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
cp /usr/src/ws/bin/WebServer /usr/bin/
cp /usr/src/ws/scripts/webserver.service /etc/systemd/system/
mkdir -p /etc/webserver/
cp /usr/src/ws/scripts/webserver.conf /etc/webserver/
systemctl daemon-reload
echo "> Completed"
echo "Start daemon: systemctl start webserver.service"
echo "Status daemon: systemctl status webserver.service"
