#include "common.h"
#include "utils.h"

bool ioopm_int_eq(ioopm_elem_t int1, ioopm_elem_t int2)
{
  return((int1.int_elem - int2.int_elem) == 0);
}


bool ioopm_string_eq(ioopm_elem_t str1, ioopm_elem_t str2)
{
    if(strcmp(str1.ptr_elem, str2.ptr_elem) == 0)
    {
        return true;
    }
    return false; 
}

bool ioopm_shelf_eq(ioopm_elem_t shelf1, ioopm_elem_t shelf2)
{
  if(strcmp((shelf1.ptr_elem), (shelf2.ptr_elem)) == 0)
  {
    return(is_shelf_location((shelf1.ptr_elem)));
  } else {
    return false;
  }
}

bool ioopm_merch_eq(ioopm_elem_t merch1, ioopm_elem_t merch2)
{
  return(merch1.merch_elem == merch2.merch_elem);
}

bool ioopm_comp_func(ioopm_elem_t a, ioopm_elem_t b) //För ints
{
  if(a.int_elem >= b.int_elem)
  {
    return true;
  }
  return false;
}

bool ioopm_comp_string_func(ioopm_elem_t a, ioopm_elem_t b)
{
  return(strcmp(a.ptr_elem, b.ptr_elem)==0);
}

int ioopm_string_sum_hash(ioopm_elem_t e)
{
  char *str = e.ptr_elem;
  int result = 0;
  do
    {
      result += *str;
    }
  while (*++str != '\0');
  return abs(result); //ÄNDRADE TILL ABS
}

int ioopm_merch_sum_hash(ioopm_elem_t merch)
{
  char *str = merch.merch_elem->name;
  int result = 0;
  do
    {
      result += *str;
    }
  while (*++str != '\0');
  return abs(result); //ÄNDRADE TILL ABS
}

int ioopm_hash_f(ioopm_elem_t key)
{
  if(key.int_elem < 0)
  {
    key.int_elem = abs(key.int_elem);
  } 
  
  return key.int_elem;
}

