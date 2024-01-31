Simple warehouse program 
IOOPM assignment 2
October 2022
Created by Edvard Axelman & Simon Eriksson
##########################################


RUNNING THE PROGRAM
###################
To run the program, compile it by typing: make frontend and then typing ./frontend
If you want to check the program with valgrind, simply type: make memtest_frontend
If you want to run the unit tests for warehouse.c, type: make memtest_warehouse_unit_tests


COVERAGE
########
Coverage was captured using gcov & lcov 
To reproduce these values simply type: make coverage, this runs a file simulating frontend usage
If you want the coverage for the warehouse unit tests, simply type: make warehouse_unit_tests_coverage

Line coverage frontend.c: 93.2%
Function coverage frontend.c: 100%

Line coverage shelves.c: 100%
Function coverage shelves.c: 100%

Line coverage warehouse.c: 99.2%
Function coverage warehouse.c: 100%

Line coverage warehouse_unit_tests: 98.6%
Function coverage warehouse_unit_tests: 100%


FILES
#####
warehouse.c - Made by Edvard Axelman & Simon Eriksson
warehouse.h - Made by Edvard Axelman & Simon Eriksson

shelves.c - Made by Edvard Axelman & Simon Eriksson
shelves.h - Made by Edvard Axelman & Simon Eriksson

frontend.c - Made by Edvard Axelman & Simon Eriksson
frontend.h - Made by Edvard Axelman & Simon Eriksson

hash_table.c - Made by Edvard Axelman & Nawwar Al Far
hash_table.h - Made by Edvard Axelman & Nawwar Al Far

common.c - Made by Edvard Axelman & Nawwar Al Far
common.h - Made by Edvard Axelman & Nawwar Al Far

utils.c - Made by Simon Eriksson 
utils.h - Made by Simon Eriksson

iterator.c - Made by Simon Eriksson & Einar Johansson
iterator.h - Made by Simon Eriksson & Einar Johansson

linked_list.c - Made by Simon Eriksson & Einar Johansson
linked_list.h - Made by Simon Eriksson & Einar Johansson

