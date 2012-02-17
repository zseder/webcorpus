#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>


#include "splitcode.h"

#define MAX_STR_LEN			512

using namespace std;


enum
{
	INITIAL = 0,
	CONTENT,
        META
};

char header[MAX_STR_LEN];
char footer[MAX_STR_LEN];

typedef vector<string> Doc;

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
    for ( size_t i=0; i<d.size(); ++i )
        h = hash( d[i], h );
    return h;
}



typedef map<Hash,long> Hashes;

Hashes g_hashes;

// Duplicate detection.
bool process_duplicate( const long& id, Doc& content )
{
    Hashes& hashes = g_hashes;

    bool toWrite = true;
    Hash h = hash(content);
    // cerr << "Doc " << id << " has hash " << h << endl;
    
    Hashes::iterator it = hashes.lower_bound(h);
    if ( (it!=hashes.end()) && (it->first==h) )
    {
        cerr << "Docs " << it->second << " and " << id << " are identical." << endl;
        toWrite = false;
    }
    else
        hashes.insert( it, make_pair(h,id) );
    
    return toWrite;
}


typedef multimap<Hash,long> HashesToIds;
typedef map<long,Hash> IdsToHashes;

HashesToIds g_hashesToIds;
IdsToHashes g_idsToHashes;

inline bool process( const long& id, Doc& content )
{
    return process_duplicate( id, content );
}

int main(int argc, char **argv)
{
	int state = INITIAL;

	char buf[BUFSIZ];;
	
	sprintf(header, "DOCSTART %s", SPLITCODE);
	sprintf(footer, "DOCEND %s", SPLITCODE);

	// get lines until eof
        
    Doc content;
    long id;

	while(fgets(buf, BUFSIZ, stdin))
	{
		// jump over the metadata
		if(state == INITIAL)
		{
			// if content comes
			if(strncmp(buf, header, SPLITCODELEN + 9) == 0)
			{
				state = CONTENT;
                id = atoi(buf+SPLITCODELEN +9);
			}
		}
		// parse content
		else
		{
			// check for the end of content
			if(strncmp(buf, footer, SPLITCODELEN + 7) == 0)
			{
                if (process(id,content))
                {
                    cout << header << " " << id << "\n";
                    for ( size_t i=0; i!=content.size(); ++i )
                        cout << content[i];
                    cout << footer << "\n\n";
                }

			    state = INITIAL;
                content.clear();
			}
			else
                content.push_back(buf);
		}
	}

	return 0;
}

