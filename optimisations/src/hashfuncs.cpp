#include "hashfuncs.h"

size_t get_hash_crc32 (Elemt elem)
{
    unsigned int hash = 0;
    int size = 8 * sizeof (size_t);
    for (int i = 0; elem[i] != '\0'; i++)
        hash = crc_table[elem[i] ^ (hash >> (size - 8))] ^ (hash << 8);

    return (size_t) hash;
}