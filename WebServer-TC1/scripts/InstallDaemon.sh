#! /bin/bash

echo "> Installing WebServer Daemon"
sudo cp ../bin/WebServer /usr/bin/
sudo cp webserver.service /etc/systemd/system/
sudo mkdir -p /etc/webserver/
sudo cp webserver.conf /etc/webserver/
sudo systemctl daemon-reload
echo "> Completed"
echo "Start daemon: sudo systemctl start webserver.service"
echo "Status daemon: sudo systemctl status webserver.service"
