CC              = gcc
CFLAGS          = -g $(OPT_FLAGS)
CUNIT_LINK      = -lcunit
BIN_DIR         = ./bin

SOURCES         = $(wildcard src/*/*/*.c src/*/*.c src/*.c)
SOURCES_NODIR   = $(notdir $(SOURCES))
OBJECTS         = $(patsubst %.c,%.o,$(SOURCES))
OBJECT_NODIR    = $(notdir $(OBJECTS))
HEADERS         = $(wildcard src/*/*/*.h src/*/*.h src/*.h)

HEAP_SRC        		= $(wildcard src/heap/**/*.c src/heap/*.c)
ROOTSET_SRC     		= $(wildcard src/find_root_set/**/*.c src/find_root_set/*.c)
HEADER_SRC      		= $(wildcard src/header/**/*.c src/header/*.c)
BITVECTOR_SRC   		= $(wildcard src/bitvector/**/*.c src/bitvector/*.c)
UTIL_SRC        		= $(wildcard src/util/**/*.c src/util/*.c)
TRAVERSE_SRC			= $(wildcard src/traverse_and_copy/**/*.c src/traverse_and_copy/*.c)
ALLOCATE_SRC			= $(wildcard src/allocate/**/*.c src/allocate/*.c)
GARBAGE_COLLECTOR_SRC	= $(wildcard src/garbage_collector/**/*.c src/garbage_collector/*.c)

TEST_SRC        = $(wildcard test/*.c)
TEST_OBJECTS    = $(wildcard test/*.o) $(OBJECTS)
TEST_NODIR      = $(notdir $(TEST_OBJECTS))
TEST_BINS       = $(patsubst %.c,%,$(TEST_SRC))

all: memtest

# COMPILING

demo: $(OBJECTS) $(DEMOOBJECTS)
	$(MAKE) -C ./demo/inlupp2 run

%.o:  %.c
	$(CC) $(CFLAGS) $^ -lm -c

%: %.o
	$(CC) $(CFLAGS) $^ -lm -c

gc: $(SOURCES)
	$(CC) $(CFLAGS) $^ -lm -c

garbage_collector: $(GARBAGE_COLLECTOR_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

heap: $(HEAP_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

rootset: $(ROOTSET_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

header: $(HEADER_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

traverse: $(TRAVERSE_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

allocate: $(ALLOCATE_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

bitvector: $(BITVECTOR_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

util: $(UTIL_SRC)
	$(CC) $(CFLAGS) $^ -lm -c

bin:
	mkdir -p $(BIN_DIR)

# UNIT TESTS

%_tests: test/%_tests.o $(OBJECTS) | bin
	$(CC) $(CFLAGS) $(notdir $<) $(TEST_NODIR) -lm -o $(BIN_DIR)/$@ $(CUNIT_LINK);\

%_test: %_tests
	./$(BIN_DIR)/$<
	make clean

compile_tests:
	for i in $(TEST_BINS); do \
			make $${i#*/};\
		done

test: compile_tests
	for i in $(TEST_BINS); do \
			$(BIN_DIR)/$${i#*/};\
		done
	make clean


# MEMORY TESTS WITH VALGRIND

memtest: compile_tests
	for i in $(TEST_BINS); do \
			valgrind --leak-check=full --undef-value-errors=no ./$(BIN_DIR)/$${i#*/};\
        done 
	make clean


# DEBUGGING WITH GDB

%_gdb: %_tests
	gdb ./$(BIN_DIR)/$<


# FORMAT CODE

format:
	indent -i4 -npsl -di0 -br -nce -d0 -cli0 -npcs -nfc1 -nut  $(SOURCES)

# COVERAGE TESTING WITH GCOV AND LCOV

# NOTE: No Guarantee that any of the below is working just yet. Hopefully we can update it when
# we start looking into coverage 

gcov: $(OBJECTS) $(TEST_OBJECTS)
	$(CC) $(CFLAGS) $(SOURCES) $(TEST_SRC) -coverage -o $(BIN_DIR)/gc_tests_gcov $(CUNIT_LINK)
	$(BIN_DIR)/gc_tests_gcov
	rm -f gc_tests*.gcno gc_tests*.gcda utils.gcda utils.gcno
	gcov -abcfu $(SOURCES_NODIR)
	make clean 

lcov:  $(OBJECTS) $(TEST_OBJECTS) bin
	$(CC) $(CFLAGS) $(SOURCES) $(TEST_SRC) -coverage -o $(BIN_DIR)/gc_tests_gcov $(CUNIT_LINK)
	$(BIN_DIR)/gc_tests_gcov
	rm -f gc_tests*.gcno gc_tests*.gcda
	lcov --directory . -c -o app.info
	genhtml -o ./coverage app.info
	make clean

special_gc:
	gcc -Wall -g src/header/format_string/format_string.c -lm -c
	gcc -Wall -g src/allocate/allocate.c -lm -c
	gcc -Wall -g src/allocation_map/allocation_map.c -lm -c
	gcc -Wall -g src/bitvector/bitvector.c -lm -c
	gcc -Wall -g src/find_root_set/find_root_set.c -lm -c
	gcc -Wall -g src/garbage_collector/garbage_collector.c -lm -c
	gcc -Wall -g src/header/header.c -lm -c
	gcc -Wall -g src/heap/heap.c -lm -c
	gcc -Wall -g src/page_map/page_map.c -lm -c
	gcc -Wall -g src/page/page.c -lm -c
	gcc -Wall -g src/traverse_and_copy/traverse_and_copy.c -lm -c
	gcc -Wall -g src/util/binary_util.c -lm -c
	gcc -Wall -g src/util/string_util.c -lm -c
	gcc -Wall -g src/gc.c -lm -c
	mkdir -p ./bin
	gcc -Wall -g test/gc_tests.c -lm -c
	gcc -Wall -g gc_tests.o format_string.o allocate.o allocation_map.o bitvector.o find_root_set.o garbage_collector.o header.o heap.o page_map.o page.o traverse_and_copy.o binary_util.o string_util.o gc.o -lm -o ./bin/gc_tests -lcunit
	./bin/gc_tests

# HOUSEKEEPING

clean:
	rm -rf *.o */*.o app.info bin coverage/*
	rm -f *.h.gch *.gcho *.gcov *.gcda *.gcno
	$(MAKE) -C ./demo/inlupp2 clean

cleaner:
	rm -rf *.c~ */*.c~ */*/*.c~

# valgrind --leak-check=full --track-origins=yes ./bin/find_root_set_tests
#make clean


.PHONY: all bin test memtest gdb gcov lcov clean cleaner

