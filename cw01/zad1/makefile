CC = gcc
FLAGS = -Wall
LIB_NAME = lib

all: static shared clean

static:
	$(CC) $(FLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o

	ar rcs $(LIB_NAME).a $(LIB_NAME).o

shared:
	$(CC) -fPIC $(FLAGS) -c $(LIB_NAME).c -o $(LIB_NAME).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB_NAME).so $(LIB_NAME).o

clean:
	rm -f *.o

purge: clean
	rm -f *.so
	rm -f *.a