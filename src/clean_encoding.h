
#define VALID_CONT(x) (unsigned char)x <= 191 && (unsigned char)x >= 128
#define NUM_ENCODINGS 4

const char* possible_encodings[NUM_ENCODINGS] = { "ISO-8859-1", "ISO-8859-15", "windows-1250", "windows-1252" };

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

enum operations { COUNT, REPLACE };

int count_or_replace_with_iconv(iconv_t myconv, const char* input, size_t first, size_t second, int operation, const long freq[], char* output)
{
    // initializing arguments for iconv
    //   for first character 
    char* first_in_buf = (char*) malloc((first)*sizeof(char));
    strncpy(first_in_buf, &input[0], first);
    char* first_in_ptr = first_in_buf;
    char first_out_buf[1];
    char* first_out_ptr = first_out_buf;
    size_t first_l = first;
    size_t first_ol = 1;

    //   for second character 
    char* second_in_buf = (char*) malloc(second*sizeof(char));
    strncpy(second_in_buf, &input[first], second);
    char* second_in_ptr = second_in_buf;
    char second_out_buf[1];
    char* second_out_ptr = second_out_buf;
    size_t second_l = second;
    size_t second_ol = 1;

    // calling iconv
    iconv(myconv, &first_in_ptr, &first_l, &first_out_ptr, &first_ol);
    free(first_in_buf);
    first_in_buf = NULL;
    iconv(myconv, &second_in_ptr, &second_l, &second_out_ptr, &second_ol);
    free(second_in_buf);
    second_in_buf = NULL;
    if (first_ol == 1 || second_ol == 1)
        return 0;

    // saving iconv output
    unsigned char first_byte = (unsigned char) first_out_buf[0];
    unsigned char second_byte = (unsigned char) second_out_buf[0];

    // check if 2 result bytes could have been bytes of a utf-8 character
    if (first_byte >= 192 && first_byte < 223 &&
        second_byte >= 128 && second_byte < 191)
    {
        if (operation == COUNT)
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

            iconv(myconv, &back_in_ptr, &back_in_l, &back_out_ptr, &back_out_l);
            free(back_in_buf);
            back_in_buf = NULL;
            if (back_out_l < 1)
            {
                return freq[256 * first_byte + second_byte];
            }
        }
        else if (operation == REPLACE)
        {
            output[0] = first_out_buf[0];
            output[1] = second_out_buf[0];
            return 1;
        }
    }
    return 0;
}

void fix_utf8_encoding(const char* input, const long freq[], int scores[], iconv_t iconvs[], int best_iconv, char* result)
{
    unsigned int i;
    unsigned int l = strlen(input);
    int pos_in_result = 0;
    for(i=0; i < l;)
    {
        char first = get_next_utf8_char_len(&input[i], (unsigned char)(l - i));
        if (first > 1 && first + i < l)
        {
            char second = get_next_utf8_char_len(&input[i+first], (unsigned char)(l - i - first));
            if (second <= 0) 
            {
                // we want to skip this character and we trigger this with a second=1
                second = 1;
            }
            else if (second > 1)
            {
                if (best_iconv == -1)
                {
                    int local_scores[NUM_ENCODINGS];
                    int iconv_i;
                    for (iconv_i=0; iconv_i < NUM_ENCODINGS; iconv_i++)
                        local_scores[iconv_i] = 0;
                    for (iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
                    {
                        local_scores[iconv_i] += count_or_replace_with_iconv(iconvs[iconv_i], &input[i], first, second, COUNT, freq, NULL);
                    }
                    int max = 0;
                    for (iconv_i=0; iconv_i < NUM_ENCODINGS; iconv_i++)
                        max = max > local_scores[iconv_i] ? max : local_scores[iconv_i];
                    for (iconv_i=0; iconv_i < NUM_ENCODINGS; iconv_i++)
                        if (local_scores[iconv_i] == max)
                            scores[iconv_i] += 1;
                }
                else
                {
                    char new_char[2];
                    count_or_replace_with_iconv(iconvs[best_iconv], &input[i], first, second, REPLACE, NULL, &new_char[0]);
                    strncpy(&result[pos_in_result], new_char, 2);
                    pos_in_result += 2;
                }
            }
            else
            {
                // second == 1 && first > 1
                if (best_iconv != -1)
                {
                    strncpy(&result[pos_in_result], &input[i], first + second);
                    pos_in_result += first + second;
                }
            }
            i += second;
        }
        else if (first == 1)
        {
            // first == 1
            if (best_iconv != -1)
            {
                strncpy(&result[pos_in_result], &input[i], 1);
                pos_in_result += 1;
            }
        }
        else
        {
            // invalid utf8 char, now skip it simply
            first = 1;
        }
        i += first;
    }
}

void fix_1byte_encoding(const char* input, const long freq[], iconv_t iconvs[], char* output)
{
    unsigned int i;
    unsigned int l = strlen(input);
    int pos_in_result = 0;
    for(i=0; i < l;i++)
    {
        char next_utf8_len = get_next_utf8_char_len(&input[i], (unsigned char)(l - i));
        if (next_utf8_len == 0)
        {
            // skip character, its not valid and messes up data

        }
        else if (next_utf8_len > 0)
        {
            // valid utf8 character, we skip it, we care about invalid bytes
            strncpy(&output[pos_in_result], &input[i], next_utf8_len);
            pos_in_result += next_utf8_len;
            i += next_utf8_len - 1; // only next_utf8_len-1, because for increases "i" with 1
        }
        else
        {
            char best[2];
            long best_score = 0;
            //iconv
            for (int enc_i = 0; enc_i < NUM_ENCODINGS; enc_i++)
            {
                char in_char[1];
                in_char[0] = input[i];
                char* in_ptr = in_char;
                char new_char[6];
                char* new_ptr = new_char;
                size_t in_l = 1;
                size_t out_l = 6;

                iconv(iconvs[enc_i], &in_ptr, &in_l, &new_ptr, &out_l);
                // we care about 2-byte chars right now
                if (out_l == 4)
                {
                    int score = freq[256*(unsigned char)new_char[0] + (unsigned char)new_char[1]];
                    if (score > best_score)
                    {
                        best_score = score;
                        best[0] = new_char[0];
                        best[1] = new_char[1];
                    }
                }
            }
            if (best_score > 0)
            {
                strncpy(&output[pos_in_result], best, 2);
                pos_in_result += 2;
            }
            else
            {
                strncpy(&output[pos_in_result], &input[i], 1);
                pos_in_result += 1;
            }
        }
    }
}


