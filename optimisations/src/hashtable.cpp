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

extern "C" unsigned int asm_get_hash_crc32 (unsigned char* elem, int len);

Error hash_ctor (HashTable* hash, size_t size, unsigned int (*hash_func) (Elemt), const char* name, const char* file, const char* func, int line)
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
        list_dtor ((hash->table)[i]);

    hash->size      = 0;
    free (hash->table);

    RETURN_ERROR(CORRECT, "");
}

Error hash_fill (HashTable* hash, __m128i* words, int num_words, int* len_words)
{
    if (!hash)
        RETURN_ERROR_AND_DUMP(hash, NULL_POINTER, "Null pointer of hashtable.");

    if (!(hash->table))
        RETURN_ERROR_AND_DUMP(hash, NULL_POINTER, "Null pointer of table in hashtable.");

    for (int i = 0; i < num_words; i++)
    {
        Error error = hash_add_elem (hash, words[i], len_words[i]);
        PARSE_ERROR(hash, error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error hash_add_elem (HashTable* hash, Elemt elem, int len)
{
    size_t hash_val = asm_get_hash_crc32((unsigned char*) &elem, len) % hash->size;
    if (search_value ((hash->table)[hash_val], elem))
        RETURN_ERROR(CORRECT, "");

    Iterator it = {};
    list_push_end ((hash->table)[hash_val], elem, &it);

    RETURN_ERROR(CORRECT, "");
}

bool hash_find_elem (HashTable* hash, Elemt elem, int len)
{
    size_t hash_val = asm_get_hash_crc32((unsigned char*) &elem, len) % hash->size;
    return search_value ((hash->table)[hash_val], elem);
}

void hash_test_finding (HashTable* hash, __m128i* words, int num_words, int* len_words)
{
    for (int i = 0; i < num_words; i++)
        for (int j = 0; j < NUM_TESTS; j++)
            hash_find_elem (hash, words[i], len_words[i]);
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