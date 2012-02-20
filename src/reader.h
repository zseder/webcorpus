#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>

#include "splitcode.h"
#define MAX_STR_LEN			512

using namespace std;
typedef vector<string> Doc;

int get_doc(FILE* f, Doc* doc)
{
	char header[MAX_STR_LEN];
	char footer[MAX_STR_LEN];
	char buf[BUFSIZ];
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);

	// get lines until eof
	while(fgets(buf, BUFSIZ, f))
	{
        // if a header came, we except content from now on
        if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
        {
            if(doc->size() != 0)
            {
                cerr << "Calling get_doc with non-empty vector!";
                exit(-1);
            }
            doc->push_back(buf);
        }
		// parse content
        else if(strncmp(buf, footer, SPLITCODELEN + 7) == 0)
        {
            doc->push_back(buf);
            return 1;
        }
        else
        {
            int l = strlen(buf);
            if (l >= BUFSIZ -1)
                cerr << "Very long line with BUFSIZ=" << BUFSIZ << " skipped." <<endl;
            if (l == 1)
                continue;
            doc->push_back(buf);
        }
	}
	return -1;
}
