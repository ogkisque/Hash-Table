#include "list.h"
#include "hashtable.h"
#include "hashfuncs.h"

const char*     FILE_NAME       = "txt/text.txt";
const char*     TEST_FILE_NAME  = "txt/test.txt";
const size_t    HASHTABLE_SIZE  = 997;
const int       NUM_TEST_WORDS  = 105911;

char*   read_file       (const char* file_name);
char**  parse_buffer    (char* buffer);
void    words_dtor      (char** words);

int main ()
{
    char*  buffer       = read_file     (FILE_NAME);
    char*  buffer_test  = read_file     (TEST_FILE_NAME);
    char** test_words   = parse_buffer  (buffer_test);
    
    HashTable hash = {};
    MAKE_HASH (&hash, HASHTABLE_SIZE, get_hash_gnu);
    hash_fill (&hash, buffer);

    hash_test_finding (&hash, test_words, NUM_TEST_WORDS);

    hash_dtor (&hash);
    words_dtor (test_words);
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
    fclose (file);
    return buffer;
}

char** parse_buffer (char* buffer)
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