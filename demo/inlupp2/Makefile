CC=gcc
#FLAGS=-Wall -g --coverage
FLAGS=$(PROJECTFLAGS)
COMMON=common.o linked_list.o iterator.o hash_table.o warehouse.o utils.o\
shelves.o
PROF=common.c common.h linked_list.c linked_list.h iterator.c iterator.h\
hash_table.c hash_table.h warehouse.h warehouse.c shelves.c shelves.h\
utils.h utils.c frontend.c frontend.h

PROJECTFLAGS	=-g --coverage
PROJECTSOURCES     	= $(wildcard ../../src/*/*/*.c ../../src/*/*.c ../../src/*.c)
PROJECTOBJECTS     	= $(patsubst %.c,%.o,$(PROJECTSOURCES))

test: warehouse_unit_tests #shelves_unit_tests #hash_table_unit_tests\
linked_list_unit_tests iterator_unit_tests

run: frontend.o $(COMMON) $(PROJECTOBJECTS)
	$(CC) $(PROJECTFLAGS) $< $(notdir $(COMMON) $(PROJECTOBJECTS)) -lm -o demo.o
	clear
	valgrind --leak-check=full --undef-value-errors=no ./demo.o

frontend: frontend.o $(COMMON)
	$(CC) $(FLAGS) $^ -o $@

common.o: common.c common.h
	$(CC) $(FLAGS) $< -lm -c

%.o: %.c %.h common.o
	$(CC) $(FLAGS) $< -lm -c

%_unit_tests: %_unit_tests.c %.o $(COMMON)
	$(CC) $(FLAGS) $^ -lcunit -o $@
#	./$@ #OBS! Kommer ta bort executeable-filen vid krasch, dåligt vid segfault!

%_unit_tests_coverage: %_unit_tests.c $(COMMON)
	$(CC) $(FLAGS) $^ -lcunit  -o $@
	./$@ < test.txt
	lcov -c -d . -o coverage.info
	genhtml coverage.info -o out

%_coverage: %.c $(COMMON)
	$(CC) $(FLAGS) $^ -lcunit  -o $@
	./$@ < test.txt
	lcov -c -d . -o coverage.info
	genhtml coverage.info -o out

memtest_%: % 
	valgrind --leak-check=full ./$<

coverage: clean frontend_coverage #shelves_unit_tests_coverage #\
linked_list_unit_tests_coverage hash_table_unit_tests_coverage iterator_unit_tests_coverage

memtest: memtest_warehouse_unit_tests #memtest_shelves_unit_tests #\
memtest_iterator_unit_tests memtest_hash_table_unit_tests memtest_linked_list_unit_tests

memrun: frontend
	valgrind --leak-check=full ./$< < test.txt > $@.txt

clean:
	rm -rf out/ *.dSYM/
	rm -f gmon.out *profile.txt
	rm -f *.o *_tests *_prof.txt *coverage* *.gc* frontend

#freq-count: $(COMMON) freq-count.c
#	$(CC) $(FLAGS) -O0 $^ -o $@

#freq-count-prof: $(PROF) freq-count.c
#	$(CC) $(FLAGS) -O0 -pg $^ -o $@

#profiling: freq-count-prof
#	./$< 16k-words.txt
#	gprof $< gmon.out > 16k_prof.txt
#	./$< 10k-words.txt
#	gprof $< gmon.out > 10k_prof.txt
#	./$< 1k-long-words.txt
#	gprof $< gmon.out > 1k-long_prof.txt
#	./$< small.txt
#	gprof $< gmon.out > small_prof.txt

.PHONY: memrun memtest test clean profiling coverage