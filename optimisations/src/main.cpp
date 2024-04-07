#include "list.h"
#include "hashtable.h"
#include "hashfuncs.h"

const char*     FILE_NAME       = "txt/text.txt";
const char*     TEST_FILE_NAME  = "txt/test.txt";
const size_t    HASHTABLE_SIZE  = 997;
const int       NUM_TEST_WORDS  = 105911;

char*   read_file       (const char* file_name);
char**  parse_buffer    (char* buffer, int* len_words);
void    words_dtor      (char** words);

int main ()
{
    char*  buffer           = read_file     (FILE_NAME);
    char*  buffer_test      = read_file     (TEST_FILE_NAME);
    int*   len_test_words   = (int*) calloc (NUM_TEST_WORDS, sizeof (int));
    char** test_words       = parse_buffer  (buffer_test, len_test_words);
    
    HashTable hash = {};
    MAKE_HASH (&hash, HASHTABLE_SIZE, get_hash_crc32);
    hash_fill (&hash, buffer);

    hash_test_finding (&hash, test_words, NUM_TEST_WORDS, len_test_words);

    hash_dtor (&hash);
    words_dtor (test_words);
    free (buffer);
    free (len_test_words);
}

char* read_file (const char* file_name)
{
    FILE* file = fopen (file_name, "r");
    fseek (file, 0, SEEK_END);
    size_t count = (size_t) ftell (file);
    rewind (file);

    char* buffer = (char*) calloc (count + 1, sizeof (char));
    fread (buffer, sizeof (char), count, file);
    fclose (file);
    return buffer;
}

char** parse_buffer (char* buffer, int* len_words)
{
    char word[WORD_MAX_SIZE] = "";
    char** words = (char**) calloc (NUM_TEST_WORDS, sizeof (char*));

    for (int i = 0; *buffer != '\0'; i++)
    {
        sscanf (buffer, "%s", word);
        int len = strlen (word);
        buffer += len + 1;

        char* word1 = (char*) calloc (len, sizeof (char));
        strncpy (word1, word, len);
        words[i] = word1;
        len_words[i] = len;
    }

    return words;
}

void words_dtor (char** words)
{
    for (int i = 0; i < NUM_TEST_WORDS; i++)
    {
        free (words[i]);
    }
    
    free (words);
}