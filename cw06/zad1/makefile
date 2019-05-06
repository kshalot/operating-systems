CC = gcc
FLAGS = -Wall

all: server client

server:
	$(CC) $(FLAGS) server.c chat.c -o server

client:
	$(CC) $(FLAGS) client.c chat.c -o client

clean:
	rm -rf *.o server client
