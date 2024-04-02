#ifndef HASHFUNCS
#define HASHFUNCS

#include "list.h"

size_t get_hash_zero        (Elemt elem);
size_t get_hash_ascii1      (Elemt elem);
size_t get_hash_length      (Elemt elem);
size_t get_hash_sum_ascii   (Elemt elem);
size_t get_hash_aver_ascii  (Elemt elem);
size_t get_hash_rol         (Elemt elem);
size_t get_hash_ror         (Elemt elem);
size_t get_hash_gnu         (Elemt elem);

#endif // HASHFUNCS