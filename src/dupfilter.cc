#include <string>
#include <set>
#include "reader.h"

typedef long Hash;

Hash hash( const string& s )
{
    Hash h = 0;
    for ( size_t i=0; i!=s.size(); ++i )
    {
        h *= 31;
        h += s[i];
    }
    return h;
}

typedef set<Hash> Hashes;

Hashes g_hashes;

// Duplicate detection.
bool process_duplicate( Doc* doc )
{
    Hashes& hashes = g_hashes;
    bool toWrite = true;
    Hash h = hash(doc->text);
    
    Hashes::iterator it;
    it = g_hashes.find(h);
    if (it != g_hashes.end())
        toWrite = false;
    else
        hashes.insert( h );
    
    return toWrite;
}

inline bool process( Doc* doc )
{
    return process_duplicate( doc );
}

int main(int argc, char **argv)
{
    Doc* doc = new Doc();
	while(get_doc(stdin, doc)>0)
	{
        if (process(doc))
        {
            cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
            cout << doc->text;
            cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
        }
        delete doc;
        doc = new Doc();
	}
	return 0;
}

