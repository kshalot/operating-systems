CC = gcc
FLAGS = -Wall
MAIN = main

compile: clean
	$(CC) -o $(MAIN) $(MAIN).c
	mkdir testdir1
	mkdir testdir1/testdir2
	touch testdir1/file1
	touch testdir1/testdir2/file2
	ln -s main.c testdir1/symlink
	./$(MAIN) .
	make clean

clean:
	rm -rf testdir1
