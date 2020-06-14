#used compiler
CC=gcc
#used short options (here: directory path with header files)
CFLAGS=-I. -std=c99

#target file: dependencies
#[tab] command
all: parallel_min_max process_memory parallel_sum
	 clear
	 
parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)
	
utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

process_memory : 
	$(CC) -o process_memory process_memory.c $(CFLAGS)

Sum.o: Sum.h
	$(CC) -o Sum.o -c Sum.c $(CFLAGS)
	mkdir static
	ar rcs ./static/lib_Sum.a Sum.o

parallel_sum: Sum.o utils.o
	$(CC) -o parallel_sum.o -c parallel_sum.c $(CFLAGS) -pthread
	$(CC) parallel_sum.o utils.o Sum.o -L/projects/dorodon-os_lab_2019/lab4/src/static -l_Sum -o ./statically_linked $(CFLAGS) -pthread

clean :
	rm -f utils.o find_min_max.o parallel_min_max process_memory Sum.o ./static/lib_Sum.a parallel_sum.o
	rm -r static
	clear