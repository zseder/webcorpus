/**
 * Keeps only those sentences that
 * 1. has only one [.:], and their position is at the end;
 * 2. has only one [!?], and their position is at the end, and previos line
 *   was a good sentence, see (1)
 *
**/
#include <cstdlib>

#include "reader.h"

using namespace std;

enum
{
	GOODSENTENCE,
	BADSENTENCE
};

int main(int argc, char **argv)
{
    Doc doc;
    char* ch;
	while(get_doc(stdin, &doc)>0)
	{
        int state = BADSENTENCE;
        cout << doc[0];
        unsigned int line_num;
        for(line_num = 1; line_num != doc.size() - 1; line_num++)
        {
            const string& line = doc[line_num];
            char* cstr = (char*)malloc((line.size()+1)*sizeof(char));
            if(!cstr)
            {
                cerr << "Malloc error!\n";
                cerr << "Size: " << (line.size()+1)*sizeof(char) << endl;
                exit(-1);
            }
            strcpy(cstr, line.c_str());
            // sentence is good if last char before line end is [.:] and there were no [.:] before
            if((ch = strpbrk(cstr, ".:")) != NULL && *(ch+1) == '\n')
            {
                state = GOODSENTENCE;
                cout << cstr;
            }
            // sentence is also good if there was a good sentence before, and if the last char before line end is [!?] and there were no [!?] before
            else if(state == GOODSENTENCE && (ch = strpbrk(cstr, "!?")) != NULL && *(ch+1) == '\n')
            {
                state = BADSENTENCE;
                cout << cstr;
            }
            else
            {
                state = BADSENTENCE;
            }
            free(cstr);
		}
        cout << doc[doc.size() - 1];
        doc.clear();
	}
	return 0;
}
