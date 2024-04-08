#include <smmintrin.h>

#include "list.h"
#include "hashtable.h"
#include "hashfuncs.h"

const char*     FILE_NAME       = "txt/text.txt";
const char*     TEST_FILE_NAME  = "txt/test.txt";
const size_t    HASHTABLE_SIZE  = 997;
const int       NUM_TEST_WORDS  = 105911;
const int       NUM_WORDS       = 31009;

char*       read_file       (const char* file_name);
__m128i*    parse_buffer    (char* buffer, int num_words, int* len_words);

int main ()
{
    char*       buffer          = read_file     (FILE_NAME);
    int*        len_words       = (int*) calloc (NUM_WORDS, sizeof (int));
    __m128i*    words           = parse_buffer  (buffer, NUM_WORDS, len_words);
    int*        len_test_words  = (int*) calloc (NUM_TEST_WORDS, sizeof (int));
    char*       buffer_test     = read_file     (TEST_FILE_NAME);
    __m128i*    test_words      = parse_buffer  (buffer_test, NUM_TEST_WORDS, len_test_words);

    HashTable hash = {};
    MAKE_HASH (&hash, HASHTABLE_SIZE, get_hash_crc32);
    hash_fill (&hash, words, NUM_WORDS, len_words);

    hash_test_finding (&hash, test_words, NUM_TEST_WORDS, len_test_words);

    hash_dtor (&hash);
    free (test_words);
    free (words);
    free (buffer);
    free (buffer_test);
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

__m128i* parse_buffer (char* buffer, int num_words, int* len_words)
{
    char word[WORD_MAX_SIZE] = "";
    __m128i* words = (__m128i*) calloc (num_words, sizeof (__m128i));

    for (int i = 0; i < num_words; i++)
    {
        sscanf (buffer, "%s", word);
        int len = strlen (word);
        buffer += len + 1;
        len_words[i] = len;

        char word1[16] = {};
        strncpy (word1, word, 16);
        words[i] = _mm_set_epi8 (word1[15], word1[14], word1[13], word1[12],
                                 word1[11], word1[10], word1[9],  word1[8],
                                 word1[7],  word1[6],  word1[5],  word1[4],
                                 word1[3],  word1[2],  word1[1],  word1[0]);
    }

    return words;
}