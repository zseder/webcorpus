#include <string>
#include <set>
#include "reader.h"

typedef long Hash;

Hash hash( const string& s, Hash context=0 )
{
    Hash h = context;
    for ( size_t i=0; i!=s.size(); ++i )
    {
        h *= 31;
        h += s[i];
    }
    return h;
}

Hash hash( const Doc& d )
{
    Hash h = 0;
    // skipping header and footer
    for ( size_t i=1; i<d.size()-1; ++i )
        h = hash( d[i], h );
    return h;
}

typedef set<Hash> Hashes;

Hashes g_hashes;

// Duplicate detection.
bool process_duplicate( Doc& content )
{
    Hashes& hashes = g_hashes;
    bool toWrite = true;
    Hash h = hash(content);
    
    Hashes::iterator it;
    it = g_hashes.find(h);
    if (it != g_hashes.end())
        toWrite = false;
    else
        hashes.insert( h );
    
    return toWrite;
}

inline bool process( Doc& content )
{
    return process_duplicate( content );
}

int main(int argc, char **argv)
{
    Doc doc;
	while(get_doc(stdin, &doc)>0)
	{
        if (process(doc))
        {
            Doc::iterator it;
            for(it = doc.begin(); it != doc.end(); it++)
                cout << *it;
        }
        doc.clear();
	}
	return 0;
}

