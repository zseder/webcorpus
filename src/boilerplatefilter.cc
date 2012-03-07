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