#include "hashfuncs.h"

size_t get_hash_gnu (Elemt elem)
{
    size_t hash = 5381;
    for (int i = 0; elem[i] != '\0'; i++)
        hash = ((hash << 5) + hash) + elem[i];

    return hash;
}