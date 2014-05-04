CC    = g++
FILES = main.cpp
FLAGS = -fpermissive -lpthread -lm -std=c++11

all:	
	$(CC) -o keyLogger $(FILES) $(FLAGS) $(GTK_LIBS) $(GTK_FLAGS) 
