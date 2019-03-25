CC = gcc
FLAGS = -Wall

tests: compile
	mkdir archive
	touch list
	echo -e "testfile\n" > list
	touch testfile
	./tester testfile 1 4 10
	./monitor list 20 1 1 1000000000000000

compile:
	$(CC) $(FLAGS) -o monitor monitor.c
	$(CC) $(FLAGS) -o tester tester.c

clean:
	rm list testfile

purge: clean
	rm -rf archive monitor tester
