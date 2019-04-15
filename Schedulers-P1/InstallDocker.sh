#!/bin/bash

#************************************************************
#	Instituto Tecnológico de Costa Rica
#	Computer Engineering
#
#	Programmer: Esteban Agüero Pérez (estape11)
#
#	Last update: 12/04/2019
#
#	Operating Systems Principles
#	Professor. Diego Vargas
#
#************************************************************

if [ $(which docker | grep -c "/docker") -eq 0 ];
then
	echo -e '\e[33m> Installing Docker <\e[39m'

	echo -e '\e[44m>>> UPDATING REPOSITORIES <<< \e[49m'
	sudo apt-get update -y
	sudo apt-get install \
		apt-transport-https \
		ca-certificates \
		curl \
		gnupg-agent \
		software-properties-common -y

	echo -e '\e[44m>>> SETTING UP DOCKER REPOSITORY <<< \e[49m'
	curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
	sudo apt-key fingerprint 0EBFCD88 -y
	sudo add-apt-repository \
	   "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
	   $(lsb_release -cs) \
	   stable" -y

	echo -e '\e[44m>>> UPDATING REPOSITORIES <<< \e[49m'
	sudo apt-get update -y

	echo -e '\e[44m>>> INSTALLING DOCKER <<< \e[49m'
	sudo apt-get install docker-ce docker-ce-cli containerd.io -y

	echo -e '\e[33m> Completed <\e[39m'
else
	echo -e '\e[33m> Docker already installed <\e[39m'
fi
