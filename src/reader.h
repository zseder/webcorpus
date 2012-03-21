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
 * Simple header file for reading documents in our format with splitcode
 * */

#include <cstdio>
#include <sstream>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>

#include "splitcode.h"
#define MAX_STR_LEN			512
#define MY_BUFSIZ 65536

using namespace std;

typedef struct {
    long id;
    string text;
} Doc;

int get_doc(FILE* f, Doc* doc)
{
	char header[MAX_STR_LEN];
	char footer[MAX_STR_LEN];
	char buf[MY_BUFSIZ];
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);
    stringstream docstream;

	// get lines until eof
	while(fgets(buf, MY_BUFSIZ, f))
	{
        // if a header came, we except content from now on
        if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
        {
            int id = atoi(buf+SPLITCODELEN +9);
            doc->id = id;
        }
        else if(strncmp(buf, footer, SPLITCODELEN + 7) == 0)
        {
            doc->text = docstream.str();
            return 1;
        }
		// content
        else
        {
            int l = strlen(buf);
            if (l >= MY_BUFSIZ -1)
                cerr << "Very long line with MY_BUFSIZ=" << MY_BUFSIZ << " skipped." <<endl;
            else if (l == 1)
                continue;
            else
            {
                docstream << buf;
            }
        }
	}
	return -1;
}
