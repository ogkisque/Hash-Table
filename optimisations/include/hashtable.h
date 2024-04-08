#ifndef HASHTABLE
#define HASHTABLE

#include <smmintrin.h>
#include <stdint.h>

#include "list.h"
#include "error.h"

struct HashTable
{
    List**          table;
    size_t          size;
    unsigned int    (*hash_func) (__m128i);

    const char* name;
    const char* file;
    const char* func;
    int         line;
};

#define MAKE_HASH(hash, size, hash_func) \
        hash_ctor (hash, size, hash_func, #hash, __FILE__, __func__, __LINE__)

const size_t LIST_DEF_SIZE = 16;
const size_t WORD_MAX_SIZE = 100;
const int    NUM_TESTS     = 500;

Error   hash_ctor           (HashTable* hash, size_t size, unsigned int (*hash_func) (__m128i), const char* name, const char* file, const char* func, int line);
Error   hash_dtor           (HashTable* hash);
void    hash_dump           (HashTable* hash, Error error);
void    hash_print_error    (Error error);
Error   hash_fill           (HashTable* hash, __m128i* words, int num_words, int* len_words);
Error   hash_add_elem       (HashTable* hash, Elemt elem, int len);
bool    hash_find_elem      (HashTable* hash, Elemt elem, int len);
void    hash_test_finding   (HashTable* hash, __m128i* words, int num_words, int* len_words);

#endif // HASHTABLE