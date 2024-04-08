#include "hashfuncs.h"

unsigned int get_hash_crc32 (Elemt elem)
{
    unsigned int hash = 0;
    int size = 8 * sizeof (size_t);
    uint8_t val[16] = {};
    memcpy (val, &elem, sizeof (val));

    for (int i = 0; elem[i] != '\0'; i++)
        hash = crc_table[val[i] ^ (hash >> (size - 8))] ^ (hash << 8);

    return (size_t) hash;
}