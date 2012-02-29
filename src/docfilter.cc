#include "reader.h"
#define MIN_LINES	4 

using namespace std;

int main(int argc, char **argv)
{
    Doc doc;
	while(get_doc(stdin, &doc) > 0)
	{
        // MIN_LINES + header + footer
        if (doc.size() >= MIN_LINES + 2)
        {
            Doc::iterator it;
            for(it = doc.begin(); it != doc.end(); it++)
                cout << *it;
        }
        doc.clear();
	}
	return 0;
}
