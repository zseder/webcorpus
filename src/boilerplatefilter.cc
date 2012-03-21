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
 * Iterates through paragraphs in a parsed html document
 * and counts hash for every one of them and if one
 * comes up too many times (argv[1]), it throws away them
 *
 * */
#include <map>

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

int main(int argc, char **argv)
{
    map<Hash,int> hashes;
    Doc* doc = new Doc();
    int bound = atoi(argv[1]);
	while (get_doc(stdin, doc)>0)
	{
        cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
        size_t pos = doc->text.find("<p>");
        size_t prev = 0;
        string paragraph;
        while (pos != string::npos)
        {
            int start = doc->text.find_first_not_of(" \t", prev);
            int end = doc->text.find_last_not_of(" \n", pos) - start;
            //cout << prev << " " << pos << " " << start << " " << end << endl;
            paragraph = doc->text.substr(start, end);
            if (paragraph.size() > 0)
            {
                Hash h = hash(paragraph);
                map<Hash,int>::iterator it = hashes.find(h);
                if (it == hashes.end())
                {
                    //instert it
                    hashes[h] = 0;
                }
                else
                {
                    if (hashes[h] < bound)
                        hashes[h]++;
                }
                if (hashes[h] < bound)
                {
                    // print paragraph
                    cout << paragraph << "<p>";
                }
            }
            prev = pos+3;
            pos = doc->text.find("<p>", prev);
        }
        cout << endl;
        cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
        delete doc;
        doc = new Doc();
    }
    return 0;
}
