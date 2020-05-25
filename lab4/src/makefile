#used compiler
CC=gcc
#used short options (here: directory path with header files)
CFLAGS=-I. -std=c99

#target file: dependencies
#[tab] command
all: parallel_min_max

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm -f utils.o find_min_max.o parallel_min_max