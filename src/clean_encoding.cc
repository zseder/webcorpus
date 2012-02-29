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
    Doc doc;
    // first create 2-byte utf8 statistics
    long freq[65536];
    int i;
    for (i=0; i < 65536; i++)
        freq[i] = 0;

    int scores[NUM_ENCODINGS];
    for (i=0; i < NUM_ENCODINGS; i++)
        scores[i] = 0;

    FILE* f = fopen(argv[1], "r");
	while(get_doc(f, &doc) > 0)
	{
        unsigned int line_num;
        for(line_num = 1; line_num != doc.size() - 1; line_num++)
        {
            const string& line = doc[line_num];
            add_freq(&freq[0], line.c_str());
        }
        doc.clear();
	}

    for (i=0; i < NUM_ENCODINGS; i++)
        iconvs[i] = iconv_open(possible_encodings[i], "utf-8");
    for (i=0; i < NUM_ENCODINGS; i++)
        reverse_iconvs[i] = iconv_open("utf-8", possible_encodings[i]);
	while(get_doc(stdin, &doc) > 0)
	{
        Doc::iterator it;
        for(it = doc.begin(); it != doc.end(); it++)
        {
            fix_utf8_encoding((char*)it->c_str(), &freq[0], &scores[0], iconvs, -1, NULL);
        }
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
                cout << doc[0];

                for(unsigned int line_num = 1; line_num<doc.size()-1; line_num++)
                {
                    char* result = (char*) calloc(doc[line_num].size() * 2, sizeof(char));
                    fix_utf8_encoding(doc[line_num].c_str(), &freq[0], &scores[0], iconvs, i, result);
                    cout << result;
                    free(result);
                    result = NULL;
                }

                cout << doc[doc.size()-1];
                break;
            }
            else if (max == 0)
            {
                // check for 1byte characters, that are not valid utf-8,
                // guess it's valid encoding and convert it to utf8
                for (unsigned int line_num = 0; line_num < doc.size(); line_num++)
                {
                    // utf max len supposed to be 6
                    char* fixed = (char*) calloc(doc[line_num].size() * 6 + 1, sizeof(char));
                    fix_1byte_encoding(doc[line_num].c_str(), &freq[0], reverse_iconvs, fixed);
                    cout << fixed;
                    free(fixed);
                    fixed = NULL;
                }
                break;

            }
        }
        doc.clear();
	}
	return 0;
}
