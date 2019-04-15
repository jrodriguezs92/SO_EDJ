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

# Starts the syslog
service rsyslog start

# Copy "WebServer" executable in /usr/bin/
cp /usr/src/ws/bin/WebServer /usr/bin/

# Copy Daemon service file in /etc/systemd/system/
cp /usr/src/ws/scripts/webserver.service /etc/systemd/system/

# Create root folder for web server
mkdir -p /etc/webserver/

# Copy config file in /etc/webserver/
cp /usr/src/ws/scripts/config.conf /etc/webserver/

# Reload Daemon system
systemctl daemon-reload

# Start Daemon service
systemctl start webserver.service

echo "> Completed"
echo "Stop daemon: systemctl stop webserver.service"
echo "Status daemon: systemctl status webserver.service" 
