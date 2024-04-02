#ifndef LIST_HEADER
#define LIST_HEADER

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "colors.h"
#include "error.h"

typedef char* Elemt;

#define MAKE_LIST(list, start_size, need_realloc_inc, need_realloc_dec) \
        make_list (list, start_size, need_realloc_inc, need_realloc_dec, #list, __FILE__, __func__, __LINE__)
#define DOT

struct List;

struct Iterator
{
    ssize_t index;
    List* list;
};

Iterator    search_value    (List* list, Elemt value);
Iterator    prev_it         (Iterator it);
Iterator    next_it         (Iterator it);
Iterator    begin_it        (List* list);
Iterator    end_it          (List* list);
Error       get_value       (Iterator* it, Elemt* value);
Error       set_value       (Iterator* it, Elemt value);
Error       list_push_begin (List* list, Elemt value, Iterator* it);
Error       list_push_end   (List* list, Elemt value, Iterator* it);
Error       list_pop_begin  (List* list, Iterator* it);
Error       list_pop_end    (List* list, Iterator* it);
Error       list_insert     (List* list, Elemt value, ssize_t pos, Iterator* it);
Error       list_erase      (List* list, ssize_t pos, Iterator* it);
Error       make_list       (List** list, int start_size, bool need_realloc_inc, bool need_realloc_dec, const char* name, const char* file, const char* func, int line);
Error       list_ctor       (List* list, int start_size, bool need_realloc_inc, bool need_realloc_dec, const char* name, const char* file, const char* func, int line);
Error       list_dtor       (List* list);
int         get_size        (List* list);
void        list_dump       (List* list, Error error);
void        list_print_error(Error error);

#endif //LIST_HEADER
