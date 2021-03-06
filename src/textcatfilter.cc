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
 * runs a textcat filter on documents based on language fiven in second
 * argument and config in first argument
 * NOTE libtextcat has to be installed
 * WARNING: we attached a patch to libtextcat in src
**/
#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <stdlib.h>

extern "C" {
    void* textcat_Init(const char*, float, int);
    char *textcat_Classify(void *, const char *, size_t);
}
#include "splitcode.h"
#include "reader.h"

using namespace std;

int main(int argc, char **argv)
{
    char* conf = argv[1];
    char lang[MAX_STR_LEN];
    sprintf(lang, "[%s]", argv[2]);
    float textcat_confidence_limit = atof(argv[3]);
	
    // textcat max candidates is now constans 2
    // textcat_Init will work only if libtextcat is patched with
    // attached patch
    void *h = textcat_Init( conf, textcat_confidence_limit, 2 );

    Doc* doc = new Doc();
	while(get_doc(stdin, doc) > 0)
	{
        char* cstr = new char [doc->text.size()+1];
        strcpy(cstr, doc->text.c_str());
        int len = strlen(cstr);
        char* result = textcat_Classify(h, cstr, 10000 < len ? 10000 : len);

        if(strcmp(result, lang))
        {
            cerr << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
            cerr << doc->text;
            cerr << "DOCEND " << SPLITCODE << " " << doc->id << endl;
        }
        else
        {
            cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
            cout << doc->text;
            cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
        }
        delete doc;
        doc = new Doc();
	}
	return 0;
}
