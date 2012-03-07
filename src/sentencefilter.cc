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
    Doc* doc = new Doc();
	while(get_doc(stdin, doc) > 0)
	{
        cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
        int state = BADSENTENCE;
        size_t pos = doc->text.find("\n", 0);
        size_t prev_pos = 0;
        while ( pos != string::npos)
        {
            if (pos - prev_pos > 3 && (doc->text[pos-1] == '.' || doc->text[pos-1] == ':'))
            {
                state = GOODSENTENCE;
                cout << doc->text.substr(prev_pos + 1, pos - prev_pos);
            }
            else if (state == GOODSENTENCE && pos - prev_pos > 3 && (doc->text[pos-1] == '!' || doc->text[pos-1] == '?'))
            {
                state = BADSENTENCE;
                cout << doc->text.substr(prev_pos + 1, pos - prev_pos);
            }
            else
            {
                state = BADSENTENCE;
            }
            prev_pos = pos+1;
            pos = doc->text.find("\n", prev_pos);
        }
        cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
        delete doc;
        doc = new Doc();
	}
	return 0;
}
