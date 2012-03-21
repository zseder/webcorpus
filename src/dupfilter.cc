/*
Copyright 2011 Attila Zseder
Email: zseder@gmail.com

This file is part of webcorpus
url: https://github.com/zseder/webcorpus

webcorpus pipeline is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
 * Simple hash based filtering
 * */
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

