CC = gcc
CFLAGS = -Wall

all: loader trucker dispatcher

dispatcher: dispatcher.c
	$(CC) $(CFLAGS) dispatcher.c -o dispatcher

loader: loader.c utility.c utility.h
	$(CC) $(CFLAGS) loader.c utility.c -o loader

trucker: trucker.c utility.c utility.h
	$(CC) $(CFLAGS) trucker.c utility.c -o trucker

clean:	
	rm -rf *.o loader trucker dispatcher
