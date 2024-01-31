#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

bool not_empty(char *str)
{
	return strlen(str) > 0;
}

/// @brief rensar inputbuffern med getchar
void clear_in_buffer(void)
{
	int c;
	do
	{
		c = getchar();
	} while (c != '\n' && c != EOF);
}
/// @brief Längden av en string
/// @param str
/// @return int motsv längd på strängen minus null
int string_length(char *str)
{
	char c;
	int i = 0;
	do
	{
		c = str[i];
		i += 1;
	} while (c != '\0');
	return i - 1;
}
/// @brief Ger true om str endast har numeriska tecken
/// @param str
/// @return True / False
bool is_number(char *str)
{
	int i = 0;
	if (str[0] == '-')
	{
		i = 1;
	}

	for (; i < strlen(str) - 1; i += 1)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

bool is_positive_number(char *str)
{
	for (int i = 0; i < strlen(str) - 1; i += 1)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}
	return true;
}

bool in(char a, char *b)
{
	for (int i = 0; i < strlen(b); i++)
	{
		if (a == b[i])
		{
			return true;
		}
	}
	return false;
}

bool is_shelf_location(char *str)
{
	if (strlen(str) == 3) // en valid shelf-location har 3 chars
	{
		if (islower(*str))
		{
			str[0] = toupper(str[0]);
		}
		if (64 < str[0] && str[0] < 91) // kollar om första bokstaven är stora "A-Z"
		{
			if (isdigit(str[1]) && isdigit(str[2])) // kollar att det är 2 siffror efter
			{
				return true;
			}
		}
	}
	return false;
}

/// @brief printar en sträng utan radbrytning
/// @param str
void print(char *str)
{
	do
	{
		putchar(*str);
		str += 1;
	} while (*str != '\0');
	putchar('\0');
}

/// @brief printar en sträng med radbrytning
/// @param str
void println(char *str)
{
	print(str);
	putchar('\n');
}

/// @brief Läser in en sträng från buf som har maxstorlek buf_siz
/// @param buf
/// @param buf_siz
/// @return Längden på den inlästa strängen
int read_string(char *buf, int buf_siz)
{
	int cn = 0;
	char c;
	bool not_done = true;
	do
	{
		c = getchar();
		not_done = (c != '\n' && c != EOF && cn <= (buf_siz - 1));
		if (not_done)
		{
			buf[cn] = c;
			cn += 1;
		}
	} while (not_done);
	if (cn >= (buf_siz - 1))
	{
		clear_in_buffer();
	}
	buf[cn] = '\0';
	return cn;
}

/// @brief Hjälpfunktion för input med prompt.
/// @param question
/// @param check
/// @param convert
/// @return Int eller Char *
answer_t ask_question(char *question, check_func check, convert_func convert)
{
	int buf_siz = 255;
	char buf[255] = {0};
	do
	{
		print(question);
		read_string(buf, buf_siz);
	} while (!check(buf));
	return convert(buf);
}

/// @brief Hjälpfunktion för input med prompt.
/// @param question
/// @param check
/// @param convert
/// @return Int eller Char *
answer_t h_ask_question(char *question, check_func check, h_convert_func convert, heap_t *heap)
{
	int buf_siz = 255;
	char buf[255] = {0};
	do
	{
		print(question);
		read_string(buf, buf_siz);
	} while (!check(buf));
	return convert(buf, heap);
}

/// @brief Duplicates string onto heap
/// @param heap the heap
/// @param str the string to be duplicated
/// @return pointer to duplicate of str on heap
static char *h_strdup(const char *str, heap_t *heap)
{
    int str_length = 0;
    const char *current_char = str;

    while (*current_char)
    {
        str_length++;
        current_char++;
    }
    char *str_duped = h_alloc_data(heap, (size_t)(str_length + 1));

    for (int i = 0; i <= str_length; i++)
    {
        str_duped[i] = str[i];
    }
    return str_duped;
}

/// @brief Printar en sträng(fråga) och tar input.
/// @param question
/// @return String
char *ask_question_string(char *question, heap_t *heap)
{
	return h_ask_question(question, not_empty, (h_convert_func)h_strdup, heap).string_value;
}

/// @brief Printar en sträng(fråga) och tar input.
/// @param question
/// @return en Shelf (t.ex. A2 eller g354)
char *ask_question_shelf(char *question, heap_t *heap)
{
	return h_ask_question(question, is_shelf_location, (h_convert_func)h_strdup, heap).string_value;
}

/// @brief Printar en sträng(fråga) och tar input.
/// @param question
/// @return Nummer
int ask_question_int(char *question)
{
	return ask_question(question, is_number, (convert_func)atoi).int_value;
}

/// @brief Printar en sträng(fråga) och tar input.
/// @param question
/// @return Nummer
int ask_question_positive_int(char *question)
{
	return ask_question(question, is_positive_number, (convert_func)atoi).int_value;
}