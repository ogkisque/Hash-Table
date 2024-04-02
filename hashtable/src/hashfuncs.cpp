#include "hashfuncs.h"

#define ROL(x) ((x << 1) | (x >> (sizeof (x) * 8 - 1)))
#define ROR(x) ((x >> 1) | (x << (sizeof (x) * 8 - 1)))

size_t get_hash_zero (Elemt elem)
{
    return 0;
}

size_t get_hash_ascii1 (Elemt elem)
{
    return elem[0];
}

size_t get_hash_length (Elemt elem)
{
    return strlen (elem);
}

size_t get_hash_sum_ascii (Elemt elem)
{
    size_t sum = 0;
    for (int i = 0; elem[i] != '\0'; i++)
        sum += elem[i];
    return sum;
}

size_t get_hash_aver_ascii (Elemt elem)
{
    size_t sum = get_hash_sum_ascii (elem);
    size_t len = strlen (elem);

    return sum / len;
}

size_t get_hash_rol (Elemt elem)
{
    size_t hash = 0;
    for (int i = 0; elem[i] != '\0'; i++)
        hash = ROL(hash) ^ elem[i];
    
    return hash;
}

size_t get_hash_ror (Elemt elem)
{
    size_t hash = 0;
    for (int i = 0; elem[i] != '\0'; i++)
        hash = ROR(hash) ^ elem[i];
    
    return hash;
}

size_t get_hash_gnu (Elemt elem)
{
    size_t hash = 5381;
    for (int i = 0; elem[i] != '\0'; i++)
        hash = ((hash << 5) + hash) + elem[i];

    return hash;
}