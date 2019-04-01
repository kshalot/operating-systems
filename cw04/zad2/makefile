CC = gcc
FLAGS = -Wall

tests: compile
	mkdir archive
	touch list
	echo -e "testfile1 2\ntestfile2 1\n" > list
	touch testfile1 testfile2
	./tester testfile1 1 4 10
	./monitor list 15

compile:
	$(CC) $(FLAGS) -o monitor monitor.c
	$(CC) $(FLAGS) -o tester tester.c

clean:
	rm list testfile*

purge: clean
	rm -rf archive monitor tester
