CC    = g++
FILES = main.cpp node.cpp tree.cpp treecheck.cpp
FLAGS = -fpermissive -lpthread -lm -std=c++11 -lrt

all:	
	$(CC) -o keyLogger $(FILES) $(FLAGS) $(GTK_LIBS) $(GTK_FLAGS) 
