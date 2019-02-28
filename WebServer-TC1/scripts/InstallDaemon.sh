#! /bin/bash

echo "> Installing WebServer Daemon"
sudo cp ../bin/WebServer /usr/bin/
sudo cp webserver.service /usr/lib/systemd/system/
sudo mkdir -p /etc/webserver/
sudo cp webserver.conf /etc/webserver/
sudo systemctl daemon-reload
echo "> Complete"
echo "Start daemon: sudo systemctl start webserver.service"
echo "Status daemon: sudo systemctl status webserver.service"
