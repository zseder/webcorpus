/**
 * This program should detect when a page is stored in incorrect coding
 * Some web pages are in utf-8 but they are said to be in windows-125x
 * or iso-8859-??. If so, wire handle all characters as one byte chars,
 * and so a valid utf8 char in 2+ bytes are separated into 2+ individual
 * windows-.../iso-... characters. They have to be detected and fixed.
 * */
#include <iconv.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "reader.h"
#include "clean_encoding.h"


iconv_t iconvs[NUM_ENCODINGS];
iconv_t reverse_iconvs[NUM_ENCODINGS];

void add_freq(long freqs[], const char* input)
{
    int l = strlen(input);
    int i;
    for(i=0; i < l;)
    {
        char len = get_next_utf8_char_len(&input[i], (unsigned char)(l - i));
        if (len == 2)
        {
            freqs[((unsigned char)input[i])*256 + ((unsigned char)input[i+1])] += 1;
        }
        if (len >= 1)
            i += len;
        else
            i += 1;
    }
}

int main(int argc, char **argv)
{
    // first create 2-byte utf8 statistics
    long freq[65536];
    int i;
    for (i=0; i < 65536; i++)
        freq[i] = 0;

    FILE* f = fopen(argv[1], "r");
    Doc* doc = new Doc();
	while(get_doc(f, doc) > 0)
    {
        add_freq(&freq[0], doc->text.c_str());
        delete doc;
        doc = new Doc();
    }
    if (doc != NULL)
        delete doc;

    for (i=0; i < NUM_ENCODINGS; i++)
        iconvs[i] = iconv_open(possible_encodings[i], "utf-8");
    for (i=0; i < NUM_ENCODINGS; i++)
        reverse_iconvs[i] = iconv_open("utf-8", possible_encodings[i]);
    doc = new Doc();
	while(get_doc(stdin, doc) > 0)
	{
        // reset scores
        int scores[NUM_ENCODINGS];
        for (i=0; i < NUM_ENCODINGS; i++)
            scores[i] = 0;

        // compute scores of different encodings for utf8 fix
        fix_utf8_encoding((char*)doc->text.c_str(), &freq[0], &scores[0], iconvs, -1, NULL);
        int max = 0;
        for (i=0; i < NUM_ENCODINGS; i++)
            max = max > scores[i] ? max : scores[i];

        for (i=0; i < NUM_ENCODINGS; i++)
        {
            /* if there were utf8 characters that were supposed to be 1-byte chars, fix it*/
            if (max > 0 && scores[i] == max)
            {
                // do the replacing thing and exit then
                // if tie, we choose first (they always give the same result)
                cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;

                char* result = (char*) calloc(doc->text.size() * 2, sizeof(char));
                fix_utf8_encoding(doc->text.c_str(), &freq[0], &scores[0], iconvs, i, result);
                // after done, do the 1byte fixing anyways
                // TODO ugly, copy-pasted from "if (max == 0)"
                char* fixed = (char*) calloc(strlen(result) * 2 + 1, sizeof(char));
                change_utf8_char_to_more_frequent(result, &freq[0], iconvs, reverse_iconvs, fixed);
                char* fixed_2 = (char*) calloc(strlen(fixed) * 6 + 1, sizeof(char));
                fix_1byte_encoding(fixed, &freq[0], reverse_iconvs, fixed_2);
                cout << fixed_2;
                free(result);
                result = NULL;
                free(fixed);
                fixed = NULL;
                free(fixed_2);
                fixed_2 = NULL;

                cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
                break;
            }
        }
        if (max == 0)
        {
            // check for 1byte characters, that are not valid utf-8,
            // guess it's valid encoding and convert it to utf8
            // utf max len supposed to be 6
            cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
            char* fixed = (char*) calloc(doc->text.size() * 6 + 1, sizeof(char));
            fix_1byte_encoding(doc->text.c_str(), &freq[0], reverse_iconvs, fixed);
            cout << fixed;
            free(fixed);
            fixed = NULL;
            cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
        }

        delete doc;
        doc = new Doc();
	}
    if (doc != NULL)
        delete doc;
	return 0;
}
