#include <iconv.h>
#include <errno.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

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

int main()
{
    iconv_t l1_conv = iconv_open("ISO-8859-1", "utf-8");
    //iconv_t l2_conv = iconv_open("ISO-8859-15", "utf-8");
    //iconv_t w0_conv = iconv_open("windows-1250", "utf-8");
    //iconv_t w2_conv = iconv_open("windows-1252", "utf-8");

    char buf[BUFSIZ];
    int global_i = 0;
    int left = 0;
    while(fgets(&buf[left], BUFSIZ-left, stdin))
    {
        int l = strlen(buf);
        for( int i = 0; i < l;)
        {
            size_t first = get_next_utf8_char_len(&buf[i], (unsigned char)(l - i));
            if (first == 0) 
            {
                strcpy( &buf[0], &buf[i] );
                left = l-i;
                break;
            }
            if (first > 1 && first + i < l)
            {
                size_t second = get_next_utf8_char_len(&buf[i+first], (unsigned char)(l - i - first));
                if (second == 0) 
                {
                    strcpy( &buf[0], &buf[i] );
                    left = l-i;
                    break;
                }
                if (second > 1)
                {
                    char* first_in_buf = (char*) malloc((first)*sizeof(char));
                    strncpy(first_in_buf, &buf[i], first);
                    char* first_in_ptr = first_in_buf;
                    size_t first_l = first;
                    char first_out_buf[1];
                    char* first_out_ptr = first_out_buf;
                    size_t first_ol = 1;
                    int res1 = iconv(l1_conv, &first_in_ptr, &first_l, &first_out_ptr, &first_ol);

                    char* second_in_buf = (char*) malloc(second*sizeof(char));
                    strncpy(second_in_buf, &buf[i+first], second);
                    char* second_in_ptr = second_in_buf;
                    size_t second_l = second;
                    char second_out_buf[2];
                    char* second_out_ptr = second_out_buf;
                    size_t second_ol = second;
                    int res2 = iconv(l1_conv, &second_in_ptr, &second_l, &second_out_ptr, &second_ol);
                    unsigned char first_byte = (unsigned char) first_out_buf[0];
                    unsigned char second_byte = (unsigned char) second_out_buf[0];
                    if (first_byte >= 192 && first_byte < 223 &&
                        second_byte >= 128 && second_byte < 191)
                    {
                        char* back_in_buf = (char*) malloc(2*sizeof(char));
                        strncpy(back_in_buf, &first_out_buf[0], (size_t)1);
                        strncpy(back_in_buf+1, &second_out_buf[0], (size_t)1);
                        char* back_in_ptr = back_in_buf;
                        size_t back_in_l = 2;
                        char back_out_buf[1];
                        char* back_out_ptr = back_out_buf;
                        size_t back_out_l = 1;
                        if (iconv(l1_conv, &back_in_ptr, &back_in_l, &back_out_ptr, &back_out_l) >= 0)
                        {
                            printf("%s\n", back_out_buf);
                        }
                        *back_in_ptr = 0; *back_out_ptr = 0; back_out_ptr = 0;
                        free(back_in_buf);
                    }
                    first_in_ptr = 0; *first_out_ptr = 0; first_out_ptr = 0;
                    second_in_ptr = 0; *second_out_ptr = 0; second_out_ptr = 0;
                    free(first_in_buf);
                    free(second_in_buf);

                }
                i += second;
            }
            else if (first == 1)
                ;
            else
                ;
            i += first;
        }
        global_i += l;
    }
}
