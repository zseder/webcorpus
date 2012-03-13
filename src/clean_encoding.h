
#define VALID_CONT(x) (unsigned char)x <= 191 && (unsigned char)x >= 128
#define NUM_ENCODINGS 6

const char* possible_encodings[NUM_ENCODINGS] = { "ISO-8859-1", "ISO-8859-2", "ISO-8859-15", "windows-1250", "windows-1251", "windows-1252" };

char get_next_utf8_char_len(const char* buf, int max_left_bytes)
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

size_t convert_char(iconv_t myconv, const char* input_p, size_t input_l, char* outbuf, size_t out_l)
{
    char* in_buf = (char*) malloc((input_l + 1)*sizeof(char));
    strncpy(in_buf, &input_p[0], input_l);
    char* in_ptr = in_buf;
    char* out_ptr = outbuf;
    iconv(myconv, &in_ptr, &input_l, &out_ptr, &out_l);
    free(in_buf);
    in_buf = NULL;
    return out_l;
}

/**
 * Input contains 2 valid utf-8 characters, and this function has two operations:
 * COUNT: based on given @myconv, returns a frequency of a 2-byte utf-8 character.
 *        this character is created by
 *          - converting 2 input characters to 1-byte encodings -> 2 byte
 *          - we handle this 2 byte as a potential valid utf-8 char
 * REPLACE: converts input with iconv and puts into @output (based on results of
 *          previously called COUNT.
 * */
int count_or_replace_with_iconv(iconv_t myconv, const char* input, size_t first, size_t second, int operation, const long freq[], char* output)
{
    // initializing arguments for iconv
    //   for first character 
    char first_out_buf[2];
    size_t first_converted = convert_char(myconv, &input[0], first, &first_out_buf[0], 1);

    //   for second character 
    char second_out_buf[2];
    size_t second_converted = convert_char(myconv, &input[0+second], second, &second_out_buf[0], 1);
    if (first_converted == 1 || second_converted == 1)
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
            char back_in_buf[2];
            back_in_buf[0] = first_out_buf[0];
            back_in_buf[1] = second_out_buf[0];
            char back_out_buf[1];
            size_t back_converted = convert_char(myconv, &back_in_buf[0], 2, back_out_buf, 1);
            if (back_converted < 1)
                return freq[256 * first_byte + second_byte];
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

/**
 * Iterates through input, and when reaches 2 2-byte utf-8 characters after each other,
 * do one of the two followings:
 * - if best_iconv is not given (-1):
 *   runs through all iconvs, call count_or_replace_with_iconv() with COUNT operation
 * - if best_iconv is given (>=0):
 *   calls count_or_replace_with_iconv() with REPLACE and puts result into @result
 * */
void fix_utf8_encoding(const char* input, const long freq[], int scores[], iconv_t iconvs[], int best_iconv, char* result)
{
    unsigned int i;
    unsigned int l = strlen(input);
    int pos_in_result = 0;
    for(i=0; i < l;)
    {
        char first = get_next_utf8_char_len(&input[i], l - i);
        if (first > 1 && first + i < l)
        {
            char second = get_next_utf8_char_len(&input[i+first], l - i - first);
            if (second <= 0) 
            {
                // we want to skip this character and we trigger this with a second=1
                second = 1;
                if (best_iconv != -1)
                {
                    strncpy(&result[pos_in_result], &input[i], first + second);
                    pos_in_result += first + second;
                }
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
                    if (max > 0)
                        for (iconv_i=0; iconv_i < NUM_ENCODINGS; iconv_i++)
                            if (local_scores[iconv_i] == max)
                                scores[iconv_i] += 1;
                }
                else
                {
                    char new_char[2];
                    if (count_or_replace_with_iconv(iconvs[best_iconv], &input[i], first, second, REPLACE, NULL, &new_char[0]) == 1)
                    {
                        strncpy(&result[pos_in_result], new_char, 2);
                        pos_in_result += 2;
                    }
                    else
                    {
                        strncpy(&result[pos_in_result], &input[i], first + second);
                        pos_in_result += first + second;
                    }
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

/**
 * Iterates through all bytes in input, and when an invalid byte (in utf-8) is reached,
 * checks if it could be a 1-byte encoding character, and converts it to utf-8, and
 * chooses the most frequent utf-8 character of these
 * */
void fix_1byte_encoding(const char* input, const long freq[], iconv_t iconvs[], char* output)
{
    unsigned int i;
    unsigned int l = strlen(input);
    int pos_in_result = 0;
    for(i=0; i < l;i++)
    {
        char next_utf8_len = get_next_utf8_char_len(&input[i], l - i);
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

/**
 * This method is for changing any possibly bad utf-8 characters to another one,
 * when a utf-8 character is created from another encoded char, and that encoding
 * was guessed incorrectly
 * */
void change_utf8_char_to_more_frequent(const char* input, const long freq[], iconv_t fwd_iconvs[], iconv_t bwd_iconvs[], char* output)
{
    unsigned int i;
    unsigned int l = strlen(input);
    int from_scores[NUM_ENCODINGS];
    for (i = 0; i < NUM_ENCODINGS; i++)
        from_scores[i] = 0;
    int to_scores[NUM_ENCODINGS];
    for (i = 0; i < NUM_ENCODINGS; i++)
        to_scores[i] = 0;

    // check with all encodings
    for(i=0; i < l;)
    {
        char next_utf8_len = get_next_utf8_char_len(&input[i], l - i);
        if (next_utf8_len == 2)
        {
            long local_scores[NUM_ENCODINGS];
            for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
                local_scores[iconv_i] = 0;

            for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
            {
                char out_buf[1];
                size_t converted = convert_char(fwd_iconvs[iconv_i], &input[i], next_utf8_len, out_buf, 1);
                if (converted == 0)
                {
                    for (int iconv_j = 0; iconv_j < NUM_ENCODINGS; iconv_j++)
                    {
                        char reverse_out_buf[6];
                        size_t reverse_converted = convert_char(bwd_iconvs[iconv_j], &out_buf[0], 1, reverse_out_buf, 6);
                        if (reverse_converted == 4)
                        {
                            char valid_char[2];
                            valid_char[0] = reverse_out_buf[0];
                            valid_char[1] = reverse_out_buf[1];
                            if (freq[256*(unsigned char)valid_char[0] + (unsigned char)valid_char[1]] >= freq[256*(unsigned char)input[i]+(unsigned char)input[i+1]])
                            {
                                local_scores[iconv_j] += freq[256*(unsigned char)valid_char[0] + (unsigned char)valid_char[1]];
                                from_scores[iconv_i] += 1;
                            }
                            else
                            {
                                local_scores[iconv_j] -= freq[256*(unsigned char)valid_char[0] + (unsigned char)valid_char[1]];
                                from_scores[iconv_i] -= 1;

                            }
                        }
                    }
                }
            }
            int max = 0;
            for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
                max = max > local_scores[iconv_i] ? max : local_scores[iconv_i];

            if (max > 0)
                for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
                    if (local_scores[iconv_i] == max)
                        to_scores[iconv_i]++;
        }
        i += (next_utf8_len > 1 ? next_utf8_len : 1);
    }

    int pos_in_result = 0;


    // choose best
    int max = 0;
    for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
        max = max > from_scores[iconv_i] ? max : from_scores[iconv_i];
    iconv_t iconv_from = fwd_iconvs[0];
    for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
        if (from_scores[iconv_i] == max)
        {
            iconv_from = fwd_iconvs[iconv_i];
            break;
        }
    max = 0;
    for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
        max = max > to_scores[iconv_i] ? max : to_scores[iconv_i];
    iconv_t iconv_to = bwd_iconvs[0];
    for (int iconv_i = 0; iconv_i < NUM_ENCODINGS; iconv_i++)
        if (to_scores[iconv_i] == max)
        {
            iconv_to = bwd_iconvs[iconv_i];
            break;
        }
    for (i = 0; i < l;)
    {
        char next_utf8_len = get_next_utf8_char_len(&input[i], l - i);
        if (next_utf8_len > 1)
        {
            char out_buf[1];
            size_t converted = convert_char(iconv_from, &input[i], next_utf8_len, out_buf, 1);
            if (converted == 0)
            {
                char reverse_out_buf[6];
                size_t reverse_converted = convert_char(iconv_to, &out_buf[0], 1, reverse_out_buf, 6);
                if (reverse_converted == 4)
                {
                    char valid_char[2];
                    valid_char[0] = reverse_out_buf[0];
                    valid_char[1] = reverse_out_buf[1];
                    strncpy(&output[pos_in_result], valid_char, 2);
                    pos_in_result += 2;
                }
                else
                {
                    strncpy(&output[pos_in_result], &input[i], next_utf8_len);
                    pos_in_result += next_utf8_len;
                }
            }
            else
            {
                strncpy(&output[pos_in_result], &input[i], next_utf8_len);
                pos_in_result += next_utf8_len;
            }
        }
        else
        {
            strncpy(&output[pos_in_result], &input[i], 1);
            pos_in_result += 1;
        }
        i += (next_utf8_len > 1 ? next_utf8_len : 1);
    }
}
