CC = gcc
FLAGS = -Wall
REPORT_FILE = "wyniki.txt"
MAIN = main
QUANTITY = 4096

GENERATE = generate $(FILE) $(QUANTITY) $(SIZE)
SYS_COPY = copy $(FILE) $(DEST_SYS) $(QUANTITY) $(SIZE) sys
LIB_COPY = copy $(FILE) $(DEST_LIB) $(QUANTITY) $(SIZE) lib
SYS_SORT = sort $(DEST_SYS) $(QUANTITY) $(SIZE) sys
LIB_SORT = sort $(DEST_LIB) $(QUANTITY) $(SIZE) lib

SIZE = 1
FILE = generate_1
DEST_SYS = copy_sys_1
DEST_LIB = copy_lib_1
TEST_SIZE_1 := $(GENERATE) $(SYS_COPY) $(LIB_COPY) $(SYS_SORT) $(LIB_SORT)

SIZE = 4
FILE = generate_4
DEST_SYS = copy_sys_4
DEST_LIB = copy_lib_4
TEST_SIZE_4 := $(GENERATE) $(SYS_COPY) $(LIB_COPY) $(SYS_SORT) $(LIB_SORT)

SIZE = 512
FILE = generate_512
DEST_SYS = copy_sys_512
DEST_LIB = copy_lib_512
TEST_SIZE_512 := $(GENERATE) $(SYS_COPY) $(LIB_COPY) $(SYS_SORT) $(LIB_SORT)

SIZE = 1024
FILE = generate_1024
DEST_SYS = copy_sys_1024
DEST_LIB = copy_lib_1024
TEST_SIZE_1024 := $(GENERATE) $(SYS_COPY) $(LIB_COPY) $(SYS_SORT) $(LIB_SORT)

SIZE = 4096
FILE = generate_4096
DEST_SYS = copy_sys_4096
DEST_LIB = copy_lib_4096
TEST_SIZE_4096 := $(GENERATE) $(SYS_COPY) $(LIB_COPY) $(SYS_SORT) $(LIB_SORT)

SIZE = 8192
FILE = generate_8192
DEST_SYS = copy_sys_8192
DEST_LIB = copy_lib_8192
TEST_SIZE_8192 := $(GENERATE) $(SYS_COPY) $(LIB_COPY) $(SYS_SORT) $(LIB_SORT)

test: compile
	./$(MAIN) $(TEST_SIZE_1) $(TEST_SIZE_4) $(TEST_SIZE_512) $(TEST_SIZE_1024) $(TEST_SIZE_4096) $(TEST_SIZE_8192) | tee -a $(REPORT_FILE)
	make clean

compile:
	$(CC) $(FLAGS) -o $(MAIN) $(MAIN).c

clean:
	rm -f copy*
	rm -f data*
	rm -f generate*