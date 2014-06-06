#!/bin/bash

FILEPATH="/etc/keylogger.conf"
FILEPATHINIT="/etc/init/ninja.conf"
STARTCONF="start on runlevel [2345]"
STOPCONF="stop on runlevel [016]"
STARTNUMBER=2345
STOPNUMBER='016'

if [ "$(whoami)" != "root" ] 
then
	echo 'Brak wystarczajacych uprawnien do uruchomienia skryptu!'
	exit
fi

if test $# -lt 1
then
	echo 'Usage : keylogger_conf --auto {true|false}'
	echo '   or : keylogger_conf --event _liczba_'
	echo '	 or : keylogger_conf --log sciezka_do_pliku'
	echo '   or : keylogger_conf --add word1 word2 ...'
	echo '   or : keylogger_conf --rem word1 word2 ...'
	echo ''
	echo 'Powyzsze opcje mozna laczyc, np. keylogger_conf --auto true --add word1 --rem word2'
	echo '\t --auto \t: czy program ma startować automatycznie'
	echo '\t --event \t: numer zdarzenia które ma być nasłuchiwane'
	echo '\t\t  program nie bedzie dzialal poprawnie jest to zdarzenie nie bedzie zwiazane z klawiatura'
	echo '\t --log \t: konfiguracja sciezki do pliku w ktorym maja byc zapisywane zdarzenia'
	echo '\t --add \t: dodanie slow do listy wykrywania'
	echo '\t --rem \t: usuniecie slow z listy wykrywania'
	exit
fi

if !([ -f $FILEPATH ])
then
	echo "Brak pliku konfiguracyjnego, zostanie on utworzony."
	echo "auto=true\n4\n/home/keylogger.log\n------klucze------" > $FILEPATH
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
		"--reload")
			stop ninja
			start ninja
			continue
			;;

		"--auto") 
			if [ $1 = "false" -o $1 = "true" ]
			then

				if [ $1 = "false" ]
				then 
					if (grep -Fxq auto=true $FILEPATH)
					then
					echo "siema"
					sed -i 's/start on runlevel \[2345\]//g' $FILEPATHINIT
					sed -i 's/stop on runlevel \[016\]//g' $FILEPATHINIT
					fi
				else
					if (grep -Fxq auto=false $FILEPATH)
					then
					echo $STARTCONF >> $FILEPATHINIT
					echo $STOPCONF >> $FILEPATHINIT
					fi
				fi
				sed -i '1c\'"auto=$1" $FILEPATH
					
				
			else
				echo "Niepoprawny argument dla --auto!"
				exit
			fi		
		;;
		"--event")
			if [[ $1 =~ ^[0-9]+$ ]]
			then
				sed -i '2c\'$1 $FILEPATH
			else
				echo "Niepoprawny argument dla --event! == $1"
				exit
			fi
		;; 
		"--log")
			sed -i '3c\'$1 $FILEPATH		
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
