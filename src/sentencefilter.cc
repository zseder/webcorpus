/**
 * Keeps only those sentences that
 * 1. has only one [.:], and their position is at the end;
 * 2. has only one [!?], and their position is at the end, and previos line
 *   was a good sentence, see (1)
 *
**/
#include <iostream>
#include <cstdio>
#include <cstring>

#include "splitcode.h"

#define MAX_STR_LEN			512

using namespace std;

enum
{
	INITIAL = 0,
	CONTENT,
	GOODSENTENCE,
	BADSENTENCE
};

int main(int argc, char **argv)
{
	int state = INITIAL;

	char header[MAX_STR_LEN];
	char footer[MAX_STR_LEN];
	char buf[BUFSIZ];
	char *ch;
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);

	// get lines until eof
	while(fgets(buf, BUFSIZ, stdin))
	{
		if(state == INITIAL)
		{
            // if a header came, we except content from now on
			if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
			{
				state = CONTENT;
				cout << buf;
			}
		}
		// parse content
		else
		{
			// check for the end of content
			if(strncmp(buf, footer, SPLITCODELEN + 7) == 0)
			{
				state = INITIAL;
				cout << buf;
			}
			else
			{
				// sentence is good if last char before line end is [.:] and there were no [.:] before
				if((ch = strpbrk(buf, ".:")) != NULL && *(ch+1) == '\n')
				{
					state = GOODSENTENCE;
					cout << buf;
				}
				// sentence is also good if there was a good sentence before, and if the last char before line end is [!?] and there were no [!?] before
				else if(state == GOODSENTENCE && (ch = strpbrk(buf, "!?")) != NULL && *(ch+1) == '\n')
				{
					state = BADSENTENCE;
					cout << buf;
				}
				else
				{
					state = BADSENTENCE;
				}
			}
		}
	}
	return 0;
}
