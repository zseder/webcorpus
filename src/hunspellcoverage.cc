/*
Copyright 2011 Attila Zseder
Email: zseder@gmail.com

This file is part of webcorpus
url: https://github.com/zseder/webcorpus

webcorpus pipeline is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
 * Keeps only those documents, whose words are recognized by hunspell
 * and this recognition ratio (good/all words) are above a given threshold
**/
#include <stdlib.h>
#include <iconv.h>

#include "reader.h"

extern "C" {
    typedef struct Hunhandle Hunhandle;
    Hunhandle *Hunspell_create(const char * affpath, const char * dpath);
    int Hunspell_spell(Hunhandle *pHunspell, const char *);
    char *Hunspell_get_dic_encoding(Hunhandle *pHunspell);
}

#define MAX_WORD_LEN			512

using namespace std;

const char* punct= ".,!?:()[]-";

int main(int argc, char **argv)
{
    char* aff = argv[1];
    char* dic = argv[2];
	
    Hunhandle *h = Hunspell_create(aff, dic);
    char* hse = Hunspell_get_dic_encoding(h);

    iconv_t myconv = iconv_open(hse, "utf-8");

    Doc* doc = new Doc();
	while(get_doc(stdin, doc)>0)
	{
        int good = 0;
        int bad = 0;
        const string& line = doc->text;
        char* cstr = (char*)malloc((doc->text.size()+1)*sizeof(char));
        if(!cstr)
        {
            cerr << "Malloc error!\n";
            cerr << "Size: " << (doc->text.size()+1)*sizeof(char) << endl;
            exit(-1);
        }
        strcpy(cstr, line.c_str());
        char* word = strtok(cstr, " \n");
        while (word != NULL) 
        {
            int l = strlen(word);
            if (l == 1 && strpbrk(word, punct) != NULL)
                good++;
            else if (l <= MAX_WORD_LEN)
            {
                size_t inbuflen = l;
                size_t outbuflen = 4*MAX_WORD_LEN;
                {
                    char iconv_out_buf[4*MAX_WORD_LEN];
                    char* iconv_out_ptr = iconv_out_buf;
                    if (iconv(myconv, &word, &inbuflen, &iconv_out_ptr, &outbuflen) >= 0)
                    {
                        *iconv_out_ptr = 0;
                        Hunspell_spell(h, iconv_out_buf) ? good++ : bad++;
                    }
                    else
                    {
                        // error occured during encoding
                        bad++;
                    }
                }
            }
            word = strtok(NULL, " \n");
        }
        free(cstr);
        cstr = NULL;
        cout << doc->id << " " << float(good)/float(good+bad) << endl;
        delete doc;
        doc = new Doc();
	}
    iconv_close(myconv);
	return 0;
}
