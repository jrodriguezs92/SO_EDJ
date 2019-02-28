#! /bin/bash

echo "> Uninstalling WebServer Daemon"
sudo rm -f /usr/bin/WebServer
sudo rm -f /usr/lib/systemd/system/webserver.service
sudo rm -rf /etc/webserver/
sudo rm -f /etc/webserver/webserver.conf
sudo rm -f /var/log/webserver.log
sudo systemctl stop webserver.service
sudo systemctl daemon-reload
echo "> Complete"
