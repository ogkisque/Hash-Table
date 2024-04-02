#include "list.h"
#include "hashtable.h"

const char* FILE_NAME = "txt/text.txt";

char* read_file (const char* file_name);

int main ()
{
    char* buffer = read_file (FILE_NAME);

    HashTable hash = {};
    MAKE_HASH (&hash, 23, get_hash);
    hash_fill (&hash, buffer);

    hash_print_info (&hash);

    hash_dtor (&hash);  
    free (buffer);
}

char* read_file (const char* file_name)
{
    FILE* file = fopen (file_name, "r");
    fseek (file, 0, SEEK_END);
    size_t count = ftell (file);
    rewind (file);

    char* buffer = (char*) calloc (count + 1, sizeof (char));
    fread (buffer, sizeof (char), count, file);
    return buffer;
}