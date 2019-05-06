CC = gcc
FLAGS = -Wall

all: server client

server:
	$(CC) $(FLAGS) server.c chat.c -o server -lrt

client:
	$(CC) $(FLAGS) client.c chat.c -o client -lrt

clean:
	rm -rf *.o server client
