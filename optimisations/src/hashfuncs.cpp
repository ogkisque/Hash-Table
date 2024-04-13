#include "hashfuncs.h"

unsigned int get_hash_crc32 (unsigned char* elem)
{
    unsigned int hash = 0;
    int size = 8 * sizeof (unsigned int);

    for (int i = 0; elem[i] != '\0'; i++)
        hash = crc_table[elem[i] ^ (hash >> (size - 8))] ^ (hash << 8);

    return hash;
}