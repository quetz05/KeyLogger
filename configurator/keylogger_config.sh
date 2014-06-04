#!/bin/bash

FILEPATH="/etc/keylogger.conf"

if [ "$(whoami)" != "root" ] 
then
	echo 'Brak wystarczajacych uprawnien do uruchomienia skryptu!'
	exit
fi

if test $# -lt 2
then
	echo 'Usage : keylogger_conf --auto {true|false}'
	echo '   or : keylogger_conf --add word1 word2 ...'
	echo '   or : keylogger_conf --rem word1 word2 ...'
	echo ''
	echo 'Powyzsze opcje mozna laczyc, np. keylogger_conf --auto true --add word1 --rem word2'
	exit
fi

if !([ -f $FILEPATH ])
then
	echo "Brak pliku konfiguracyjnego, zostanie on utworzony."
	echo "auto=true\n------klucze------" > $FILEPATH
	echo "Plik utworzony"
fi


komenda=""

while test $# -gt 0
do
	if [[ $1 == --* ]]
	then
		komenda=$1
		shift
	fi

	case $komenda in
		"--auto") 
			if [ $1 = "false" -o $1 = "true" ]
			then
				sed -i '1c\'"auto=$1" $FILEPATH
			else
				echo "Niepoprawny argument dla --auto!"
				exit
			fi		
		;;
		"--add")
			if !(grep -Fxq $1 $FILEPATH)
			then
				echo $1 >> $FILEPATH
			fi
		;;
		"--rem") 
			if grep -Fxq $1 $FILEPATH
			then
				sed -i 's/\<'$1'\>//g' $FILEPATH
				sed -i '/./!d' $FILEPATH
			fi		
		;;
	esac
	shift
done
