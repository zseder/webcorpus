#include <sstream>
#include <bitset>

#include "reader.h"

typedef unsigned long uint64_t;

extern "C" {
    uint64_t charikar_hash64_wide( const char *data, long data_length, int stoken_size);
}

using namespace std;

int main(int argc, char **argv)
{
    Doc doc;
	while(get_doc(stdin, &doc))
	{
        stringstream docstream;
        unsigned int line_num;
        for(line_num = 1; line_num != doc.size() - 1; line_num++)
            docstream << doc[line_num];
        string docstring = docstream.str();
        uint64_t simhash = charikar_hash64_wide(docstring.c_str(), docstring.size(), 3);

        /*for ( size_t i=0; i!=doc.size(); ++i )
            cout << doc[i];
        cout << endl;*/
        cout << doc[0];
        cout << bitset<64>(simhash) << "\n";
        cout << doc[doc.size()-1];
        doc.clear();
	}

	return 0;
}

