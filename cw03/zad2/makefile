CC = gcc
FLAGS = -Wall

tests: compile
	mkdir archive
	touch list
	echo -e "testfile1 2\ntestfile2 1\n" > list
	touch testfile1 testfile2
	./tester testfile1 1 4 10
	./tester testfile2 1 10 20
	./monitor list 15 1
	./monitor list 15 0

compile:
	$(CC) $(FLAGS) -o monitor monitor.c
	$(CC) $(FLAGS) -o tester tester.c

clean:
	rm list testfile*

purge: clean
	rm -rf archive monitor tester
