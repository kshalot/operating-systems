CC = gcc
FLAGS = -Wall
MAIN = main

compile: clean
	$(CC) -o $(MAIN) $(MAIN).c
	ln -s main.c symlink
	./$(MAIN) . 2019-03-18 '=' stat
	./$(MAIN) . 2019-01-01 '<' nftw

clean:
	rm -f symlink
