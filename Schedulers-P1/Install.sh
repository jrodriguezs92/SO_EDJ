#!/bin/bash

# Author: Jeremy Rodriguez
#         Daniela Hernandez
#         Esteban Agüero
# Date: 14/04/2019
############################################################

# Script absolute path
SCRIPT=`realpath $0`
SCRIPTPATH=`dirname $SCRIPT`

# Regular expresion to check if the port as arguments are numbers
re='^[0-9]+$'

# If there is 1 argument
if [ "$#" -eq 1 ]
then

	# If the argument is the word help: print help
	if [ "$1" = 'help' ]
	then

		echo -e '\e[36m>> type ./Install.sh p1 p2 p3 p4 p5'
		echo -e 'Where p1, p2, p3, p4 and p5'
		echo -e 'refers to each server port you want to map'
		echo -e 'p1 -- Sequential server port'
		echo -e 'p2 -- Forked server port'
		echo -e 'p3 -- Threaded server port'
		echo -e 'p4 -- Pre-Forked server port'
		echo -e 'p5 -- Pre-Threaded server port \e[39m'

	# If the unique argument is different than 'help'
	else
		
		echo -e '\e[31m>> Error: type the correct arguments <<\e[39m'
		echo -e '\e[31m>> You can type <help> as argument <<\e[39m'
	
	fi

# If there is 5 parameters
elif [ "$#" -eq 5 ]
then

	# If the parameters are numbers, take it as port numbers: Installation
	if [[ $1 =~ $re ]]  &&  [[ $2 =~ $re ]] &&  [[ $3 =~ $re ]] &&  [[ $4 =~ $re ]] &&  [[ $5 =~ $re ]]
	then

		echo -e '\e[44m>>> VERIFIYNG DOCKER INSTALLATION \e[49m'
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

		################# FIFO SERVER

		echo -e '\e[44m>>> INSTALLING FIFO SERVER \e[49m'

		echo -e '\e[33m> Building Docker Image \e[39m'

		# Build image from Dockerfile.
		sudo docker build --tag=webserver_fifo $SCRIPTPATH/FIFO/

		echo -e '\e[33m> Executing Docker container \e[39m'

		# Run Docker container: 
		# in detached mode,
		# in privileged mode (for the use of systemctl commands), 
		# with friendly name "webserver", 
		# and with a defined volume.
		# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
		sudo docker run -d -p "$1":8001 --privileged --name webserver_fifo -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_fifo

		echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

		# Execute command inside the container in detached mode, to install Daemon and run it.
		sudo docker exec -d webserver_fifo ./scripts/InstallDaemon.sh

		echo -e '\e[33m> successfully completed! \e[39m'



		################# FORKED SERVER

		echo -e '\e[44m>>> INSTALLING FORKED SERVER \e[49m'

		echo -e '\e[33m> Building Docker Image \e[39m'

		# Build image from Dockerfile.
		sudo docker build --tag=webserver_forked $SCRIPTPATH/Forked/

		echo -e '\e[33m> Executing Docker container \e[39m'

		# Run Docker container: 
		# in detached mode,
		# in privileged mode (for the use of systemctl commands), 
		# with friendly name "webserver", 
		# and with a defined volume.
		# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
		sudo docker run -d -p "$2":8003 --privileged --name webserver_forked -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_forked

		echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

		# Execute command inside the container in detached mode, to install Daemon and run it.
		sudo docker exec -d webserver_forked ./scripts/InstallDaemon.sh

		echo -e '\e[33m> successfully completed! \e[39m'



		################# THREADED SERVER

		echo -e '\e[44m>>> INSTALLING THREADED SERVER \e[49m'

		echo -e '\e[33m> Building Docker Image \e[39m'

		# Build image from Dockerfile.
		sudo docker build --tag=webserver_threaded $SCRIPTPATH/Threaded/

		echo -e '\e[33m> Executing Docker container \e[39m'

		# Run Docker container: 
		# in detached mode,
		# in privileged mode (for the use of systemctl commands), 
		# with friendly name "webserver", 
		# and with a defined volume.
		# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
		sudo docker run -d -p "$3":8005 --privileged --name webserver_threaded -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_threaded

		echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

		# Execute command inside the container in detached mode, to install Daemon and run it.
		sudo docker exec -d webserver_threaded ./scripts/InstallDaemon.sh

		echo -e '\e[33m> successfully completed! \e[39m'



		################# PRE-FORKED SERVER

		echo -e '\e[44m>>> INSTALLING PRE-FORKED SERVER \e[49m'

		echo -e '\e[33m> Building Docker Image \e[39m'

		# Build image from Dockerfile.
		sudo docker build --tag=webserver_preforked $SCRIPTPATH/Pre-forked/

		echo -e '\e[33m> Executing Docker container \e[39m'

		# Run Docker container: 
		# in detached mode,
		# in privileged mode (for the use of systemctl commands), 
		# with friendly name "webserver", 
		# and with a defined volume.
		# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
		sudo docker run -d -p "$4":8007 --privileged --name webserver_preforked -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_preforked

		echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

		# Execute command inside the container in detached mode, to install Daemon and run it.
		sudo docker exec -d webserver_preforked ./scripts/InstallDaemon.sh

		echo -e '\e[33m> Successfully completed! \e[39m'



		################# PRE-THREADED SERVER

		echo -e '\e[44m>>> INSTALLING PRE-THREADED SERVER \e[49m'

		echo -e '\e[33m> Building Docker Image \e[39m'

		# Build image from Dockerfile.
		sudo docker build --tag=webserver_prethreaded $SCRIPTPATH/Pre-threaded/

		echo -e '\e[33m> Executing Docker container \e[39m'

		# Run Docker container: 
		# in detached mode,
		# in privileged mode (for the use of systemctl commands), 
		# with friendly name "webserver", 
		# and with a defined volume.
		# REFERENCE: https://hub.docker.com/r/jrei/systemd-ubuntu
		sudo docker run -d -p "$5":8009 --privileged --name webserver_prethreaded -v /sys/fs/cgroup:/sys/fs/cgroup:ro webserver_prethreaded

		echo -e '\e[33m> Installing and running Daemon inside the container \e[39m'

		# Execute command inside the container in detached mode, to install Daemon and run it.
		sudo docker exec -d webserver_prethreaded ./scripts/InstallDaemon.sh

		echo -e '\e[33m> Successfully completed! \e[39m'

	# If the parameters are words
	else

		echo -e '\e[31m> Error: Type numbers for the port map \e[39m'

	fi

# If there is a different number of arguments than 1 por 5
else
	
	echo -e '\e[31m>> Error: type the correct arguments <<\e[39m'
	echo -e '\e[31m>> You can type <help> as argument <<\e[39m'

fi