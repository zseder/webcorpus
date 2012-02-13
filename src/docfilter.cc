#include <iostream>
#include <cstdio>
#include <cstring>

#include "splitcode.h"

#define MAX_STR_LEN	512
#define MIN_LINES	4 

using namespace std;

enum
{
	INITIAL = 0,
	CONTENT,
	PASSED
};

int main(int argc, char **argv)
{
	int state = INITIAL;
	unsigned int lines = 0;

	char header[MAX_STR_LEN];
	char footer[MAX_STR_LEN];

	char buf[BUFSIZ];
	char linebuf[MIN_LINES][BUFSIZ];
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);
  
	// get lines until eof
	while(fgets(buf, BUFSIZ, stdin))
	{
		// jump over the metadata
		if(state == INITIAL)
		{
			// if content comes
			if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
			{
				lines = 0;

				strncpy(linebuf[lines], buf, sizeof(linebuf[lines]));
				lines++;

				state = CONTENT;
			}
		}
		// parse content
		else
		{
			// check for the end of content
			if(strncmp(buf, footer, SPLITCODELEN + 7) == 0)
			{
				// if any content was written then
				// write the closing tag
				if(state == PASSED)
				{
					cout << buf;
				}

				state = INITIAL;
			}
			else if(state == PASSED)
			{
			    cout << buf;
			}
			else if(lines < MIN_LINES)
			{
				strncpy(linebuf[lines], buf, sizeof(linebuf[lines]));
				lines++;
			}
			else
			{
			    for(int i = 0; i < MIN_LINES; i++)
					cout << linebuf[i];

			    cout << buf;
			    state = PASSED;
			}
		}
	}

	return 0;
}
