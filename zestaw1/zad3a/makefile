CC = gcc
FLAGS = -std=c99 -Wall
LIB = lib
MAIN = main
REPORT_FILE = "raport3a.txt"

all: static shared dynamic measurements clean

static:
	$(CC) $(FLAGS) -c $(LIB).c -o $(LIB).o
	ar rcs $(LIB).a $(LIB).o
	$(CC) $(FLAGS) $(MAIN).c -I. $(LIB).a -o $(MAIN)_static
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB).so $(LIB).o
	$(CC) $(FLAGS) -Wl,-rpath=. -L. $(LIB).so -o $(MAIN)_shared $(MAIN).c
	make clean

dynamic:
	$(CC) -fPIC $(FLAGS) -c $(LIB).c -o $(LIB).o
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB).so $(LIB).o
	$(CC) $(FLAGS) -Wl,-rpath=. -L. -o $(MAIN)_dynamic $(MAIN).c -ldl -D DLL
	make clean

clean:
	rm -f *.o
	rm -f *.a

purge: clean
	rm -f *.so
	rm -f $(MAIN)_shared
	rm -f $(MAIN)_static
	rm -f $(MAIN)_dynamic

measurements:
	printf "\t\tSTATIC VERSION\n\n\n" >> $(REPORT_FILE)
	printf "ALLOCATING BIG ARRAY, CROSS ADDING AND REMOVING A LOT OF ELEMENTS\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 100000000 add_delete_loop 10000000 | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "SHALLOW SEARCH - ONE DIRECTORY, ONE FILE FOUND, SMALL BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 100000 search_directory . makefile temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, MANY FILES, BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 100000 search_directory /usr \'*.txt\' temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "DEEP SEARCH - SOME DIRECTORIES, HUGE AMOUNT OF FILES, VERY BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 20 search_directory /usr/bin \'*\' temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "\t\tSHARED VERSION\n\n\n" >> $(REPORT_FILE)
	printf "ALLOCATING BIG ARRAY, CROSS ADDING AND REMOVING A LOT OF ELEMENTS\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 100000000 add_delete_loop 10000000 | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "SHALLOW SEARCH - ONE DIRECTORY, ONE FILE FOUND, SMALL BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 100000 search_directory . makefile temp.txt dump | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, MANY FILES, BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 100000 search_directory /usr \'*.txt\' temp.txt dump | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, HUGE AMOUNT OF FILES, VERY BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 20 search_directory /usr/bin \'*\' temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "\t\tDYNAMIC VERSION\n\n\n" >> $(REPORT_FILE)
		printf "ALLOCATING BIG ARRAY, CROSS ADDING AND REMOVING A LOT OF ELEMENTS\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 100000000 add_delete_loop 10000000 | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "SHALLOW SEARCH - ONE DIRECTORY, ONE FILE FOUND, SMALL BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 100000 search_directory . makefile temp.txt dump | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, MANY FILES, BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 100000 search_directory /usr \'*.txt\' temp.txt dump | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, HUGE AMOUNT OF FILES, VERY BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 20 search_directory /usr/bin \'*\' temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
