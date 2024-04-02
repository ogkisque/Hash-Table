#include "list.h"
#include "hashtable.h"
#include "hashfuncs.h"

const char*     FILE_NAME       = "txt/text.txt";
const size_t    HASHTABLE_SIZE  = 101;
const size_t    HASH_FUNCS_NUM  = 8;

char* read_file (const char* file_name);

int main ()
{
    char* buffer = read_file (FILE_NAME);

    size_t (*hash_funcs[HASH_FUNCS_NUM]) (char*) = {get_hash_zero,
                                                    get_hash_ascii1,
                                                    get_hash_length,
                                                    get_hash_sum_ascii,
                                                    get_hash_aver_ascii,
                                                    get_hash_rol,
                                                    get_hash_ror,
                                                    get_hash_gnu};

    const char* files_csv_name[HASH_FUNCS_NUM] = {  "data/data1.csv",
                                                    "data/data2.csv",
                                                    "data/data3.csv",
                                                    "data/data4.csv",
                                                    "data/data5.csv",
                                                    "data/data6.csv",
                                                    "data/data7.csv",
                                                    "data/data8.csv"};
    
    for (size_t i = 0; i < HASH_FUNCS_NUM; i++)
    {
        FILE* file_csv = fopen (files_csv_name[i], "w");
        HashTable hash = {};
        MAKE_HASH (&hash, HASHTABLE_SIZE, hash_funcs[i]);
        hash_fill (&hash, buffer);
        hash_print_info (&hash, file_csv);

        hash_dtor (&hash);
        fclose (file_csv);
    }

    free (buffer);
}

char* read_file (const char* file_name)
{
    FILE* file = fopen (file_name, "r");
    fseek (file, 0, SEEK_END);
    size_t count = (size_t) ftell (file);
    rewind (file);

    char* buffer = (char*) calloc (count + 1, sizeof (char));
    fread (buffer, sizeof (char), count, file);
    return buffer;
}