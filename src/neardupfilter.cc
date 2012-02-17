#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <bitset>

typedef unsigned long uint64_t;

extern "C" {
    uint64_t charikar_hash64_wide( const char *data, long data_length, int stoken_size);
}

#include "splitcode.h"

#define MAX_STR_LEN			512

using namespace std;


enum
{
	INITIAL = 0,
	CONTENT,
        META
};

char header[MAX_STR_LEN];
char footer[MAX_STR_LEN];

typedef vector<string> Doc;

int main(int argc, char **argv)
{
	int state = INITIAL;

	char buf[BUFSIZ];;
    stringstream doc;
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);

    long id;
    char myheader[MAX_STR_LEN];

	// get lines until eof
	while(fgets(buf, BUFSIZ, stdin))
	{
		// jump over the metadata
		if(state == INITIAL)
		{
			// if content comes
			if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
			{
				state = CONTENT;
                id = atoi(buf+SPLITCODELEN +9);
                strcpy(myheader, buf);
                doc.str("");
			}
		}
		// parse content
		else
		{
			// check for the end of content
			if(strncmp(buf, footer, SPLITCODELEN + 7) == 0)
			{
                char* cstr = new char [doc.str().size()+1];
                strcpy (cstr, doc.str().c_str());
                long len = strlen(cstr);
                uint64_t simhash = charikar_hash64_wide(cstr, len, 3);

                cout << myheader;
                cout << bitset<64>(simhash) << "\n";
                cout << buf;

			    state = INITIAL;
                doc.str("");
			}
			else
            {
                doc << buf;
            }
		}
	}

	return 0;
}

