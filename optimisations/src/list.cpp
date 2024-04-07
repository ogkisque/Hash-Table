#include "list.h"

#ifdef DOT
#include "Dotter.h"
#endif

#define RETURN_ERROR(code, message) \
        return Error {code, __LINE__, __FILE__, __func__, message}

#define RETURN_ERROR_AND_DUMP(list, code, message)                                          \
        {                                                                                   \
            list_dump (list, Error {code, __LINE__, __FILE__, __func__, message});          \
            return Error {code, __LINE__, __FILE__, __func__, message};                     \
        }

#define PARSE_ERROR(list, error)            \
        if (error.code != CORRECT)          \
        {                                   \
            list_dump (list, error);        \
            return error;                   \
        }

#ifdef DOT
const char* NAME_DOT        = "pic.dot";
#endif
const int   MAX_TEXT_SIZE   = 200;
const int   REALLOC_STEP    = 2;
const int   NAME_SIZE       = 50;

Error   list_realloc_increase   (List* list);
Error   list_realloc_decrease   (List* list);
Error   list_verify             (List* list);
void    fill_nodes              (List* list, ssize_t start, ssize_t end);
void    list_dtor_nodes         (List* list);
#ifdef DOT
void    list_graph_dump         (List* list, Error error);
void    dump_nodes              (List* list);
void    dump_links              (List* list);
void    dump_error              (List* list, Error error);
#endif
bool    prev_of_next            (List* list);
bool    is_cycles               (List* list);

int get_size (List* list)
{
    return list->num_elems;
}

Error list_insert (List* list, Elemt value, ssize_t pos, Iterator* it)
{
    Error error = list_verify (list);
    PARSE_ERROR(list, error);

    if (!it)
        RETURN_ERROR_AND_DUMP(list, NULL_POINTER, "Null pointer of iterator.");

    if (pos < 0 || pos > list->size - 1 || ((list->nodes[pos].prev == -1) && pos != 0))
        RETURN_ERROR_AND_DUMP(list, INCOR_POS, "Incorrect position.");

    ssize_t new_next = (list->nodes)[list->free].next;

    list->nodes[list->free].value = value;

    list->nodes[list->free].next = pos;
    list->nodes[list->free].prev = list->nodes[pos].prev;
    list->nodes[list->nodes[pos].prev].next = list->free;
    list->nodes[pos].prev = list->free;

    it->index = list->free;
    it->list  = list;
    list->free = new_next;
    list->num_elems++;

    if (list->num_elems == list->size - 1)
    {
        if (list->is_realloc_inc)
        {
            error = list_realloc_increase (list);
            PARSE_ERROR(list, error);
        }
        else
        {
            RETURN_ERROR(LIST_OVERFLOW, "There are no free elements in list.");
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error list_erase (List* list, ssize_t pos, Iterator* it)
{
    Error error = list_verify (list);
    PARSE_ERROR(list, error);

    if (!it)
        RETURN_ERROR_AND_DUMP(list, NULL_POINTER, "Null pointer of iterator.");

    if (pos <= 0 || list->nodes[pos].prev == -1 || pos >= list->size)
        RETURN_ERROR_AND_DUMP(list, INCOR_POS, "Incorrect position.");

    it->index = list->nodes[pos].next;
    it->list  = list;

    free (list->nodes[pos].value);
    list->nodes[pos].value = NULL;
    list->nodes[list->nodes[pos].prev].next = list->nodes[pos].next;
    list->nodes[list->nodes[pos].next].prev = list->nodes[pos].prev;

    list->nodes[pos].next = list->free;
    list->nodes[pos].prev = -1;
    list->free = pos;
    list->num_elems--;

    if ((list->is_realloc_dec) && (list->num_elems < list->size / REALLOC_STEP / REALLOC_STEP))
    {
        error = list_realloc_decrease (list);
        PARSE_ERROR(list, error);
    }
    RETURN_ERROR(CORRECT, "");
}

Error list_realloc_increase (List* list)
{
    Node* new_nodes = NULL;
    new_nodes = (Node*) realloc (list->nodes, list->size * REALLOC_STEP * sizeof (Node));
    if (!list->nodes)
        RETURN_ERROR(MEM_ALLOC, "Error of reallocation memory of nodes.");

    list->nodes = new_nodes;
    list->size = list->size * REALLOC_STEP;

    fill_nodes (list, list->size / REALLOC_STEP, list->size);

    RETURN_ERROR(CORRECT, "");
}

Error list_realloc_decrease (List* list)
{
    List* old_list = (List*) calloc (1, sizeof (List));
    old_list->nodes = (Node*) calloc (list->size, sizeof (Node));
    old_list->size = list->size;
    memcpy (old_list->nodes, list->nodes, list->size * sizeof (Node));

    list->size =  list->size / REALLOC_STEP;
    list->nodes = (Node*) realloc (list->nodes, list->size * sizeof (Node));
    int num = 0;
    for (ssize_t i = old_list->nodes[0].next; i != 0; i = old_list->nodes[i].next)
    {
        num++;
        list->nodes[num].value =    old_list->nodes[i].value;
        list->nodes[num].prev =     num - 1;
        list->nodes[num].next =     num + 1;
    }

    list->nodes[num].next = 0;
    list->nodes[0].next   = 1;
    list->nodes[0].prev   = num;
    list->free            = num + 1;

    fill_nodes (list, num + 1, list->size);

    free (old_list);
    RETURN_ERROR(CORRECT, "");
}

bool search_value (List* list, Elemt value)
{
    int last_index = list->num_elems;
    for (int i = 1; i < last_index; i++)
    {
        unsigned long long equal = 0;
        asm volatile(
        ".intel_syntax noprefix\n\t"
        "movq rsi, %1\n\t"
        "movq rdi, %2\n\t"
        ".next:\n"
            "mov r11b, byte ptr [rsi]\n"
            "mov r10b, byte ptr [rdi]\n"
    	    "cmp r10b, 0\n"
    	    "je .end\n"
    	    "cmp r11b, 0\n"
    	    "je .end\n"
    	    "cmp r11b, r10b\n"
    	    "jne .end\n"
    	    "inc rdi\n"
    	    "inc rsi\n"
    	    "jmp .next\n"
        ".end:\n"
            "xor rax, rax\n"
            "xor rbx, rbx\n"
            "movzx rax, r10b\n"
            "movzx rbx, r11b\n"
    	    "sub rax, rbx\n"
            "movq %0, rax\n"
        ".att_syntax"
        : "=r" (equal)
        : "r" (list->nodes[i].value), "r" (value)
        : "rax", "rbx", "rsi", "rdi", "r10", "r11"
        );
        
        if (equal == 0)
            return true;
    }
    return false;
}

int my_strcmp (const char* str1, const char* str2)
{
    int result = 0;
    asm(
        ".intel_syntax noprefix\n\t"
        "movq rsi, %1\n\t"
        "movq rdi, %2\n\t"
        ".Next:\n"
            "mov r11b, byte ptr [rsi]\n"
            "mov r10b, byte ptr [rdi]\n"
    	    "cmp r10b, 0\n"
    	    "je .done\n"
    	    "cmp r11b, 0\n"
    	    "je .done\n"
    	    "cmp r11b, r10b\n"
    	    "jne .done\n"
    	    "inc rdi\n"
    	    "inc rsi\n"
    	    "jmp .Next\n"
        ".done:\n"
            "xor rax, rax\n"
            "xor rbx, rbx\n"
            "movzx rax, r10b\n"
            "movzx rbx, r11b\n"
    	    "sub rax, rbx\n"
        ".att_syntax"
        : "=r" (result)
        : "r" (str1), "r" (str2)
        : "rax", "rbx", "rsi", "rdi", "r10", "r11"
    );
    return result;
}

Iterator prev_it (Iterator it)
{
    return Iterator {((it.list)->nodes)[it.index].prev, it.list};
}

Iterator next_it (Iterator it)
{
    return Iterator {((it.list)->nodes)[it.index].next, it.list};
}

Iterator begin_it (List* list)
{
    return Iterator {list->nodes[0].next, list};
}

Iterator end_it (List* list)
{
    return Iterator {0, list};
}

Elemt get_value (Iterator* it)
{
    return it->list->nodes[it->index].value;
}

Error set_value (Iterator* it, Elemt value)
{
    if (it->index <= 0 || it->index >= it->list->size || it->list->nodes[it->index].prev == -1)
        RETURN_ERROR_AND_DUMP(it->list, INCOR_POS, "Incorrect index.");

    if (!it)
        RETURN_ERROR_AND_DUMP(it->list, NULL_POINTER, "Null pointer of iterator.");

    strcpy (it->list->nodes[it->index].value, value);
    RETURN_ERROR(CORRECT, "");
}

Error list_push_begin (List* list, Elemt value, Iterator* it)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_insert (list, value, list->nodes[0].next, it);
}

Error list_push_end (List* list, Elemt value, Iterator* it)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_insert (list, value, 0, it);
}

Error list_pop_begin (List* list, Iterator* it)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_erase (list, list->nodes[0].next, it);
}

Error list_pop_end (List* list, Iterator* it)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_erase (list, list->nodes[0].prev, it);
}

Error make_list (List** list, int start_size, bool need_realloc_inc, bool need_realloc_dec, const char* name, const char* file, const char* func, int line)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of list.");

    *list = (List*) calloc (1, sizeof (List));

    if (!list)
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory of list.");

    Error error = list_ctor (*list, start_size, need_realloc_inc, need_realloc_dec, name, file, func, line);
    return error;
}

Error list_ctor (List* list, int start_size, bool need_realloc_inc, bool need_realloc_dec, const char* name, const char* file, const char* func, int line)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");

    list->size = start_size;
    list->is_realloc_inc = need_realloc_inc;
    list->is_realloc_dec = need_realloc_dec;
    list->nodes = (Node*) calloc (list->size, sizeof (Node));

    if (!(list->nodes))
        RETURN_ERROR(MEM_ALLOC, "Failed memory allocation of nodes");

    fill_nodes (list, 1, list->size);

    list->nodes[0].value =  NULL;
    list->nodes[0].next =   0;
    list->nodes[0].prev =   0;
    list->free =            1;
    list->num_elems =       0;
    list->name =            name;
    list->file =            file;
    list->func =            func;
    list->line =            line;

    RETURN_ERROR(CORRECT, "");
}

Error list_dtor (List* list)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");

    list_dtor_nodes (list);

    free (list->nodes);
    list->nodes =       NULL;
    list->size =        -1;
    list->free =        -1;
    list->num_elems =   -1;
    free (list);
    RETURN_ERROR(CORRECT, "");
}

void list_dtor_nodes (List* list)
{
    for (Iterator it1 = begin_it (list), it2 = end_it (list);
        it1.index != it2.index;
        it1 = next_it (it1))
    {
        Elemt val = get_value (&it1);
        if (val)
        {
            free (val);
            val = NULL;
        }
    }
}

Error list_verify (List* list)
{
    if (!list)                                          RETURN_ERROR(NULL_POINTER,      "Null pointer of list.");
    if (!(list->nodes))                                 RETURN_ERROR(NULL_POINTER,      "Null pointer of nodes.");
    if (list->nodes[0].value != NULL)                   RETURN_ERROR(INCOR_ZERO_ELEM,   "Zero element in nodes is not INT_MAX.");
    if (prev_of_next (list))                            RETURN_ERROR(PREV_OF_NEXT,      "Previous element of next element is not equal to the current one");
    if (is_cycles (list))                               RETURN_ERROR(CYCLES,            "There are cycles in list.");
    if (list->num_elems >= list->size)                  RETURN_ERROR(ELEMS_MORE_SIZE,   "There are more elements in the list than its size");
    RETURN_ERROR(CORRECT, "");
}

bool prev_of_next (List* list)
{
    for (ssize_t i = 0; i < list->size; i++)
        if (list->nodes[i].prev != -1)
            if (list->nodes[list->nodes[i].next].prev != i)
                return true;
    return false;
}

bool is_cycles (List* list)
{
    int kol = 0;
    for (ssize_t i = list->nodes[0].next; i != 0; i = list->nodes[i].next)
        kol++;
    return kol != list->num_elems;
}

void list_dump (List* list, Error error)
{
    printf (RED_COL);
    printf ("-------------------------------------\n");
    if (error.code != CORRECT)
    {
        list_print_error (error);
        printf ("Error in list: %s\n"
                "Called from file: %s, func: %s, line: %d\n",
                list->name, list->file, list->func, list->line);
    }
    printf ("Size - %d\n"
            "Number of elements - %d\n",
            list->size, list->num_elems);
    printf (YELLOW_COL);
    printf ("Data:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%s [%d] ", list->nodes[i].value, i);
    printf ("\n");
    printf (BLUE_COL);
    printf ("Next:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%ld [%d] ", list->nodes[i].next, i);
    printf ("\n");
    printf (GREEN_COL);
    printf ("Prev:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%ld [%d] ", list->nodes[i].prev, i);
    printf ("\n");
    printf (RED_COL);
    printf ("Free = %ld\n", list->free);
    printf ("-------------------------------------\n");
    printf (OFF_COL);
}

void list_print_error (Error error)
{
    printf ("Error: %s\n"
            "Code: %d\n"
            "File: %s, function: %s, line: %d\n",
            error.message,
            error.code,
            error.file, error.func, error.line);
}

#ifdef DOT
void list_graph_dump (List* list, Error error)
{
    dtBegin (NAME_DOT);
    dump_nodes (list);
    dump_links (list);
    if (error.code != CORRECT)
        dump_error (list, error);
    dtEnd ();
    dtRender (NAME_DOT);
}

void dump_nodes (List* list)
{
    char text[MAX_TEXT_SIZE] = "";
    dtNodeStyle ().shape        ("box")
                  .style        ("rounded, filled")
                  .fontcolor    ("black");

    for (ssize_t i = 0; i < list->size; i++)
    {
        sprintf (text, "Index: [%lld]\n"
                       "Value: %s\n"
                       "Prev:  [%lld]\n"
                       "Next:  [%lld]\n",
                       i, list->nodes[i].value, list->nodes[i].prev, list->nodes[i].next);
        if (list->nodes[i].prev == -1)
            dtNodeStyle ().fillcolor ("#FFA07A");
        else if (i == 0)
            dtNodeStyle ().fillcolor ("#FFD700");
        else
            dtNodeStyle ().fillcolor ("#90EE90");

        dtNode ((int) i, text);
    }
}

void dump_links (List* list)
{
    dtLinkStyle ().style ("bold")
                  .color ("#4682B4");
    for (ssize_t i = 0; i < list->size; i++)
    {
        if (list->nodes[i].next < list->size)
            dtLink ((int) i, (int) list->nodes[i].next, "");
    }

    dtLinkStyle ().style ("dashed")
                  .color ("#00FFFF");
    for (ssize_t i = 0; i < list->size; i++)
    {
        if (list->nodes[i].prev != -1)
        {
            dtLink ((int) i, (int) list->nodes[i].prev, "");
        }
    }
}

void dump_error (List* list, Error error)
{
    char text[MAX_TEXT_SIZE] = "";
    dtNodeStyle ().shape        ("box")
                  .style        ("rounded, filled")
                  .fontcolor    ("black")
                  .fillcolor    ("#FFFF00");

    sprintf (text,
            "Error in list: %s\n"
            "Called from file: %s, func: %s, line: %d\n"
            "Error: %s\n"
            "Code: %d\n"
            "File: %s, function: %s, line: %d\n",
            list->name, list->file, list->func, list->line,
            error.message, error.code, error.file, error.func, error.line);

    dtNode (list->size + 1, text);
}
#endif

void fill_nodes (List* list, ssize_t start, ssize_t end)
{
    for (ssize_t i = start; i < end; i++)
    {
        list->nodes[i].next =  i + 1;
        list->nodes[i].prev =  -1;
        list->nodes[i].value = 0;
    }
}
