#include <iostream>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;


main()
{
// wylaczenie bufora
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

// Once the buffering is turned off, the rest is simple.
int c;
do
{
    cout << "Enter: ";
    c = cin.get();
    cout << " -> " << c << endl;
}while(c!=27);

    return 0;
}
