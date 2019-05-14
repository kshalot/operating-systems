CC = gcc
CFLAGS = -Wall -pthread

all: loader trucker dispatcher

dispatcher: dispatcher.c
	$(CC) $(CFLAGS) dispatcher.c -o dispatcher

loader: loader.c utility.c utility.h
	$(CC) $(CFLAGS) loader.c utility.c -o loader -lrt

trucker: trucker.c utility.c utility.h
	$(CC) $(CFLAGS) trucker.c utility.c -o trucker -lrt

clean:	
	rm -rf *.o loader trucker dispatcher
