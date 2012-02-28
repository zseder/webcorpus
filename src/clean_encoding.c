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

#define VALID_CONT(x) (unsigned char)x <= 191 && (unsigned char)x >= 128

char get_next_utf8_char_len(const char* buf, unsigned max_left_bytes)
{
    char n = 0;
    unsigned char b0 = (unsigned char)buf[0];
    if (b0 < 128)
        n = 1;
    else if (b0 < 192)
        // first byte should not be this
        n = -1;
    else if (2 <= max_left_bytes && b0 < 224 &&
            VALID_CONT(buf[1]))
        n = 2;
    else if (3 <= max_left_bytes && b0 < 240 &&
            VALID_CONT(buf[1]) &&
            VALID_CONT(buf[2]))
        n = 3;
    else if (4 <= max_left_bytes && b0 < 248 &&
            VALID_CONT(buf[1]) &&
            VALID_CONT(buf[2]) &&
            VALID_CONT(buf[3]))
        n = 4;
    else if (5 <= max_left_bytes && b0 < 252 &&
            VALID_CONT(buf[1]) &&
            VALID_CONT(buf[2]) &&
            VALID_CONT(buf[3]) &&
            VALID_CONT(buf[4]))
        n = 5;
    else if (6 <= max_left_bytes && b0 < 254 &&
            VALID_CONT(buf[1]) &&
            VALID_CONT(buf[2]) &&
            VALID_CONT(buf[3]) &&
            VALID_CONT(buf[4]) &&
            VALID_CONT(buf[5]))
        n = 6;
    return n;
}

#define NUM_ENCODINGS 4

const char* possible_encodings[NUM_ENCODINGS] = { "ISO-8859-1", "ISO-8859-15", "windows-1250", "windows-1252" };
iconv_t iconvs[NUM_ENCODINGS];

void fix_encoding(const char* input)
{
    int scores[NUM_ENCODINGS];
    int i;
    for (i=0; i < NUM_ENCODINGS; i++)
        scores[i] = 0;

    int l = strlen(input);
    for(i=0; i < l;)
    {
        size_t first = get_next_utf8_char_len(&input[i], (unsigned char)(l - i));
        if (first == 0) 
        {
            // usually end of input
            break;
        }
        if (first > 1 && first + i < l)
        {
            size_t second = get_next_utf8_char_len(&input[i+first], (unsigned char)(l - i - first));
            if (second == 0) 
            {
                // usually end of input
                break;
            }
            if (second > 1)
            {
                int local_scores[NUM_ENCODINGS];
                int iconv_i;
                for (iconv_i=0; iconv_i < NUM_ENCODINGS; iconv_i++)
                    local_scores[iconv_i] = 0;
                for (iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
                {
                    // initializing arguments for iconv
                    //   for first character 
                    char* first_in_buf = (char*) malloc((first)*sizeof(char));
                    strncpy(first_in_buf, &input[i], first);
                    char* first_in_ptr = first_in_buf;
                    size_t first_l = first;
                    char first_out_buf[1];
                    char* first_out_ptr = first_out_buf;
                    size_t first_ol = 1;

                    //   for second character 
                    char* second_in_buf = (char*) malloc(second*sizeof(char));
                    strncpy(second_in_buf, &input[i+first], second);
                    char* second_in_ptr = second_in_buf;
                    size_t second_l = second;
                    char second_out_buf[1];
                    char* second_out_ptr = second_out_buf;
                    size_t second_ol = 1;

                    // calling iconv
                    int res1 = iconv(iconvs[iconv_i], &first_in_ptr, &first_l, &first_out_ptr, &first_ol);
                    if (first_ol == 1)
                        continue;
                    int res2 = iconv(iconvs[iconv_i], &second_in_ptr, &second_l, &second_out_ptr, &second_ol);
                    if (second_ol == 1)
                        continue;

                    // saving iconv output
                    unsigned char first_byte = (unsigned char) first_out_buf[0];
                    unsigned char second_byte = (unsigned char) second_out_buf[0];

                    // check if 2 result bytes could have been bytes of a utf-8 character
                    if (first_byte >= 192 && first_byte < 223 &&
                        second_byte >= 128 && second_byte < 191)
                    {
                        // creating arguments for second iconv 
                        // to check which encoding of the possible ones are
                        // most probably (based on frequencies
                        char* back_in_buf = (char*) malloc(2*sizeof(char));
                        strncpy(back_in_buf, &first_out_buf[0], (size_t)1);
                        strncpy(back_in_buf+1, &second_out_buf[0], (size_t)1);
                        char* back_in_ptr = back_in_buf;
                        size_t back_in_l = 2;
                        char back_out_buf[1];
                        char* back_out_ptr = back_out_buf;
                        size_t back_out_l = 1;

                        // calling iconv
                        if (iconv(iconvs[iconv_i], &back_in_ptr, &back_in_l, &back_out_ptr, &back_out_l) >= 0)
                        {
                            // TODO save a local score for actual character based on frequencies
                            if (back_out_l < 1)
                                printf("%d %s\n", iconv_i, back_in_buf);
                                scores[iconv_i] += 1;
                        }
                        *back_in_ptr = 0; *back_out_ptr = 0; back_out_ptr = 0;
                        free(back_in_buf);
                    }
                    first_in_ptr = 0; *first_out_ptr = 0; first_out_ptr = 0;
                    second_in_ptr = 0; *second_out_ptr = 0; second_out_ptr = 0;
                    free(first_in_buf);
                    free(second_in_buf);
                }

            }
            i += second;
        }
        else if (first == 1)
            ;
        else
            ;
        i += first;
    }
    for (int i=0; i < NUM_ENCODINGS; i++)
    {
        printf("%d\n", scores[i]);
    }
    printf("\n");
}

void add_freq(int* freqs, const char* input)
{
    int l = strlen(input);
    int i;
    for(i=0; i < l;)
    {
        size_t len = get_next_utf8_char_len(&input[i], (unsigned char)(l - i));
        if (len == 2)
        {
            freqs[input[i]*256 + input[i+1]] += 1;
        }
        if (len > 1)
            i += len;
        else
            i += 1;
    }
}

int main(int argc, char **argv)
{
    int i_;
    Doc doc;
    // first create 2-byte utf8 statistics
    int freq[65536];
    for (i_=0; i_ < 65536; i_++)
        freq[i_] = 0;
    FILE* f = fopen(argv[1], "r");
	while(get_doc(f, &doc) > 0)
	{
        unsigned int line_num;
        fprintf(stderr, doc[0].c_str());
        for(line_num = 1; line_num != doc.size() - 1; line_num++)
        {
            const string& line = doc[line_num];
            add_freq(freq, line.c_str());
        }
        doc.clear();
	}
    for (i_ = 0; i_ < 65536; i_++)
    {
        printf("%d %d\n", i_, freq[i_]);
    }
    

    for (int i_=0; i_ < NUM_ENCODINGS; i_++)
        iconvs[i_] = iconv_open(possible_encodings[i_], "utf-8");
	while(get_doc(stdin, &doc) > 0)
	{
        Doc::iterator it;
        for(it = doc.begin(); it != doc.end(); it++)
        {
            fix_encoding(it->c_str());
        }
        doc.clear();
	}
	return 0;
}
