#include "hashtable.h"

#define RETURN_ERROR(code, message) \
        return Error {code, __LINE__, __FILE__, __func__, message}

#define RETURN_ERROR_AND_DUMP(hash, code, message)                                          \
        {                                                                                   \
            hash_dump (hash, Error {code, __LINE__, __FILE__, __func__, message});          \
            return Error {code, __LINE__, __FILE__, __func__, message};                     \
        }

#define PARSE_ERROR(hash, error)            \
        if (error.code != CORRECT)          \
        {                                   \
            hash_dump (hash, error);        \
            return error;                   \
        }

Error hash_ctor (HashTable* hash, size_t size, size_t (*hash_func) (char*), const char* name, const char* file, const char* func, int line)
{
    if (!hash)
        RETURN_ERROR_AND_DUMP(hash, NULL_POINTER, "Null pointer of hashtable.");

    hash->table     = (List**) calloc (size, sizeof (List*));
    if (!(hash->table))
        RETURN_ERROR_AND_DUMP(hash, MEM_ALLOC, "Error with allocation memory for hashtable.");

    hash->size      = size;
    hash->hash_func = hash_func;
    hash->name      = name;
    hash->file      = file;
    hash->func      = func;
    hash->line      = line;

    for (size_t i = 0; i < size; i++)
    {
        List* list = NULL;
        MAKE_LIST(&list, LIST_DEF_SIZE, true, true);
        (hash->table)[i] = list;
    }

    RETURN_ERROR(CORRECT, "");
}

Error hash_dtor (HashTable* hash)
{
    if (!hash)
        RETURN_ERROR_AND_DUMP(hash, NULL_POINTER, "Null pointer of hashtable.");

    hash->hash_func = NULL;
    hash->name      = NULL;
    hash->func      = NULL;
    hash->file      = NULL;
    hash->line      = 0;

    for (size_t i = 0; i < hash->size; i++)
    {
        list_dtor ((hash->table)[i]);
    }
    hash->size      = 0;
    free (hash->table);
    hash->table     = NULL;

    RETURN_ERROR(CORRECT, "");
}

Error hash_fill (HashTable* hash, char* buffer)
{
    if (!hash)
        RETURN_ERROR_AND_DUMP(hash, NULL_POINTER, "Null pointer of hashtable.");

    if (!(hash->table))
        RETURN_ERROR_AND_DUMP(hash, NULL_POINTER, "Null pointer of table in hashtable.");

    char word[WORD_MAX_SIZE] = "";
    while (*buffer != '\0')
    {
        sscanf (buffer, "%s", word);
        buffer += strlen (word) + 1;

        Error error = hash_add_elem (hash, word);
        PARSE_ERROR(hash, error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error hash_add_elem (HashTable* hash, Elemt elem)
{
    size_t hash_val = (hash->hash_func)(elem) % hash->size;
    Iterator it = search_value ((hash->table)[hash_val], elem);
    if (it.index != -1)
        RETURN_ERROR(CORRECT, "");

    char* word1 = (char*) calloc (strlen (elem), sizeof (char));
    if (!word1)
        RETURN_ERROR(MEM_ALLOC, "Error with allocation memory for string.");
    
    strncpy (word1, elem, strlen (elem));
    list_push_end ((hash->table)[hash_val], word1, &it);

    RETURN_ERROR(CORRECT, "");
}

Error hash_print_info (HashTable* hash)
{
    if (!hash)
        RETURN_ERROR(NULL_POINTER, "Null pointer of hashtable.");

    if (!(hash->table))
        RETURN_ERROR(NULL_POINTER, "Null pointer of table in hashtable.");

    size_t size = hash->size;
    int sum = 0;
    int sum_val = 0;
    for (size_t i = 0; i < size; i++)
    {
        sum_val += i;
        sum += get_size ((hash->table)[i]);
        printf ("List [%lu] - %d elements\n", i, get_size ((hash->table)[i]));
    }

    float aver = (float) sum / size;
    printf ("Load factor = %.1f\n", aver);

    float dispersion = 0;
    for (size_t i = 0; i < size; i++)
        dispersion += (get_size ((hash->table)[i]) - aver) *
                      (get_size ((hash->table)[i]) - aver) *
                      (float) get_size ((hash->table)[i]) / sum;

    printf ("dispersion = %.1f\n", dispersion);

    RETURN_ERROR(CORRECT, "");
}

void hash_dump (HashTable* hash, Error error)
{
    printf (RED_COL);
    printf ("-------------------------------------\n");
    if (error.code != CORRECT)
    {
        hash_print_error (error);
        printf ("Error in hash: %s\n"
                "Called from file: %s, func: %s, line: %d\n",
                hash->name, hash->file, hash->func, hash->line);
    }
    printf ("Size - %lu\n", hash->size);

    printf (YELLOW_COL);
    printf ("Lists:\n");
    for (size_t i = 0; i < hash->size; i++)
        list_dump ((hash->table)[i], error);

    printf (RED_COL);
    printf ("-------------------------------------\n");
    printf (OFF_COL);
}

void hash_print_error (Error error)
{
    printf ("Error: %s\n"
            "Code: %d\n"
            "File: %s, function: %s, line: %d\n",
            error.message,
            error.code,
            error.file, error.func, error.line);
}

size_t get_hash (Elemt elem)
{
    return elem[0];
}