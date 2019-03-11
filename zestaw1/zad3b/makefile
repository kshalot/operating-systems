CC = gcc
FLAGS = -Wall
LIB = lib
MAIN = main
REPORT_FILE = "raport3b.txt"

all: static shared dynamic measurements clean

static:
	$(CC) $(FLAGS) -c $(LIB).c -o $(LIB).o -O$(opt)
	ar rcs $(LIB).a $(LIB).o
	$(CC) $(FLAGS) $(MAIN).c -I. $(LIB).a -o $(MAIN)_static -O$(opt)
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c $(LIB).c -o $(LIB).o -O$(opt)
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB).so $(LIB).o -O$(opt)
	$(CC) $(FLAGS) -Wl,-rpath=. -L. $(LIB).so -o $(MAIN)_shared $(MAIN).c -O$(opt)
	make clean

dynamic:
	$(CC) -fPIC $(FLAGS) -c $(LIB).c -o $(LIB).o -O$(opt)
	$(CC) -fPIC $(FLAGS) -shared -o $(LIB).so $(LIB).o -O$(opt)
	$(CC) $(FLAGS) -Wl,-rpath=. -L. -o $(MAIN)_dynamic $(MAIN).c -ldl -D DLL -O$(opt)
	make clean

clean:
	rm -f *.o
	rm -f *.a

purge: clean
	rm -f *.so
	rm -f $(MAIN)_shared
	rm -f $(MAIN)_static
	rm -f $(MAIN)_dynamic

measurement:
	printf "\t\tSTATIC VERSION -O$(opt)\n\n\n" >> $(REPORT_FILE)
	printf "ALLOCATING BIG ARRAY, CROSS ADDING AND REMOVING A LOT OF ELEMENTS\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 100000000 add_delete_loop 10000000 | tee -a $(REPORT_FILE)
	printf "SHALLOW SEARCH - ONE DIRECTORY, ONE FILE FOUND, SMALL BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 100000 search_directory . makefile temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, MANY FILES, BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_static create_table 100000 search_directory /usr \'*.txt\' temp.txt dump remove_block 0| tee -a $(REPORT_FILE)
	printf "DEEP SEARCH - HUGE AMOUNT OF DIRECTORIES, HUGE BLOCK\n"
	./$(MAIN)_static create_table 10 search_directory /usr \'*\' temp.txt dump remove_block 0 | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "\t\tSHARED VERSION -O$(opt)\n\n\n" >> $(REPORT_FILE)
	printf "ALLOCATING BIG ARRAY, CROSS ADDING AND REMOVING A LOT OF ELEMENTS\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 100000000 add_delete_loop 10000000 | tee -a $(REPORT_FILE)
	printf "SHALLOW SEARCH - ONE DIRECTORY, ONE FILE FOUND, SMALL BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 100000 search_directory . makefile temp.txt dump | tee -a $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, MANY FILES, BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_shared create_table 100000 search_directory /usr \'*.txt\' temp.txt dump | tee -a $(REPORT_FILE)
	printf "DEEP SEARCH - HUGE AMOUNT OF DIRECTORIES, HUGE BLOCK\n"
	./$(MAIN)_static create_table 10 search_directory /usr \'*\' temp.txt dump remove_block 0 | tee -a $(REPORT_FILE)
	printf "################################################\n\n" >> $(REPORT_FILE)
	printf "\t\tDYNAMIC VERSION -O$(opt)\n\n\n" >> $(REPORT_FILE)
		printf "ALLOCATING BIG ARRAY, CROSS ADDING AND REMOVING A LOT OF ELEMENTS\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 100000000 add_delete_loop 10000000 | tee -a $(REPORT_FILE)
	printf "SHALLOW SEARCH - ONE DIRECTORY, ONE FILE FOUND, SMALL BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 100000 search_directory . makefile temp.txt dump | tee -a $(REPORT_FILE)
	printf "DEEP SEARCH - MANY DIRECTORIES, MANY FILES, BIG BLOCK\n" >> $(REPORT_FILE)
	./$(MAIN)_dynamic create_table 100000 search_directory /usr \'*.txt\' temp.txt dump | tee -a $(REPORT_FILE)
	printf "DEEP SEARCH - HUGE AMOUNT OF DIRECTORIES, HUGE BLOCK\n"
	./$(MAIN)_static create_table 10 search_directory /usr \'*\' temp.txt dump remove_block 0 | tee -a $(REPORT_FILE)
	printf "\n\n\n\n\n" >> $(REPORT_FILE)

measurements:
	make measurement opt=0
	make measurement opt=1
	make measurement opt=2
	make measurement opt=3
