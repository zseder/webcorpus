/**
 * Keeps only those sentences that
 * TODO
 *
**/
#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>

extern "C" {
    void* textcat_Init(const char*);
    char *textcat_Classify(void *, const char *, size_t);
}
#include "splitcode.h"

#define MAX_STR_LEN			512

using namespace std;

enum
{
	INITIAL = 0,
	CONTENT
};

int main(int argc, char **argv)
{
	int state = INITIAL;

	char header[MAX_STR_LEN];
	char footer[MAX_STR_LEN];
	char buf[BUFSIZ];
    stringstream doc;

    char* conf = argv[1];
    char lang[MAX_STR_LEN];
    sprintf(lang, "[%s]", argv[2]);
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);
    char myheader[MAX_STR_LEN];
    
    void *h = textcat_Init( conf );

	// get lines until eof
	while(fgets(buf, BUFSIZ, stdin))
	{
		if(state == INITIAL)
		{
            // if a header came, we except content from now on
			if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
			{
				state = CONTENT;
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
				state = INITIAL;
                // call textcat
                char* cstr = new char [doc.str().size()+1];
                strcpy (cstr, doc.str().c_str());
                int len = strlen(cstr);
                char* result = textcat_Classify(h, cstr, 10000 < len ? 10000 : len);

                if(strcmp(result, lang))
                {
                    cerr << myheader;
                    cerr << cstr << "\n";
                    cerr << buf;
                }
                else
                {
                    cout << myheader;
                    cout << cstr << "\n";
                    cout << buf;
                }
			}
			else
			{
                doc << buf;
			}
		}
	}
	return 0;
}
