/**
 * Simple filter that throws away all documents that doesn't have at least MIN_LINES
 * sentences
 * */
#include <string>
#include "reader.h"
#define MIN_LINES	4 

using namespace std;

int main(int argc, char **argv)
{
    Doc* doc = new Doc();
	while(get_doc(stdin, doc) > 0)
	{
        int line_num = 0;
        size_t pos = doc->text.find("\n", 0);
        while (pos != string::npos)
        {
            line_num++;
            if (line_num >= MIN_LINES)
            {
                cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
                cout << doc->text;
                cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
                break;
            }
            pos = doc->text.find("\n", pos+1);
        }
        delete doc;
        doc = new Doc();
	}
	return 0;
}
