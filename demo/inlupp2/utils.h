#pragma once
#include <stdbool.h>
#include "../../src/gc.h"

/**
 * @file utils.h
 * @author Simon Eriksson and Edvard Axelman
 * @brief Contains common tools used mainly by the frontend.
 */

// -- Typer --

/// @union answer_t
/// @param int_value answer of type int
/// @param string_value answer of type char *
typedef union { 
  int   int_value;
  char *string_value;
} answer_t;

typedef bool(*check_func)(char *);
typedef answer_t(*convert_func)(char *);

typedef answer_t(*h_convert_func)(char *str, heap_t *heap);

// -- Externa funktioner? --

extern char *strdup(const char *);

// -- Funktioner --

void clear_in_buffer(void);
int read_string(char *buf, int buf_siz);
bool in(char a,char *b);
bool is_number(char *str);
bool is_shelf_location(char *str);
bool not_empty(char *str);
void println(char *str);
int string_length(char *str);
void print(char *str);
answer_t ask_question(char *question, check_func check, convert_func convert);
int ask_question_int(char *question);
int ask_question_positive_int(char *question);
char *ask_question_string(char *question, heap_t *heap);
char *ask_question_shelf(char *question, heap_t *heap);