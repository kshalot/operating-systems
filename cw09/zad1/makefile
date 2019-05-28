CC = gcc
CFLAGS = -Wall -std=c99 -pthread -lrt

all: main

main:
	$(CC) $(CFLAGS) main.c utility.c -o main

clean:	
	rm -rf *.o main
