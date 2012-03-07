#include <sstream>
#include <bitset>
#include <map>
#include <vector>
#include <algorithm>

#include "reader.h"

typedef unsigned long uint64_t;
typedef map<int, uint64_t> IdSimhashMap;
typedef map<uint64_t, vector<int> > SimhashIdMap;

extern "C" {
    uint64_t charikar_hash64_wide( const char *data, long data_length, int stoken_size);
    int hamming_dist( uint64_t a1, uint64_t a2);
}

using namespace std;

IdSimhashMap g_id_to_simhash;
SimhashIdMap g_simhash_to_id;

void search_dup_dummy()
{
    SimhashIdMap::iterator first_hash_it; 
    for (first_hash_it = g_simhash_to_id.begin(); first_hash_it != g_simhash_to_id.end(); ++first_hash_it)
    {
        SimhashIdMap::iterator second_hash_it; 
        for (second_hash_it = first_hash_it; second_hash_it != g_simhash_to_id.end(); ++second_hash_it)
        {
            if (first_hash_it != second_hash_it && hamming_dist(first_hash_it->first, second_hash_it->first) <= 3)
            {
                vector<int>::iterator id_it;
                for (id_it = g_simhash_to_id[first_hash_it->first].begin(); id_it != g_simhash_to_id[first_hash_it->first].end(); id_it++)
                    cout << *id_it << " ";
                for (id_it = g_simhash_to_id[second_hash_it->first].begin(); id_it != g_simhash_to_id[second_hash_it->first].end(); id_it++)
                    cout << *id_it << " ";
                cout << endl;
            }
        }
    }
}

vector<int> search_duplicates()
{
    vector<vector<uint64_t> > buckets;
    SimhashIdMap::iterator hash_it; 
    for (hash_it = g_simhash_to_id.begin(); hash_it != g_simhash_to_id.end(); hash_it++)
    {
        vector<vector<uint64_t> >::iterator bucket_it;
        bool inserted = false;
        for (bucket_it = buckets.begin(); bucket_it != buckets.end(); bucket_it++)
        {
            if (hamming_dist(hash_it->first, bucket_it->at(0)) <= 20)
            {
                bucket_it->push_back(hash_it->first);
                inserted = true;
                break;
            }
        }
        if (!inserted)
        {
            vector<uint64_t> new_bucket;
            new_bucket.push_back(hash_it->first);
            buckets.push_back(new_bucket);
        }
    }

    //after creating buckets, check only those who are in the same one
    vector<int> to_drop;

    vector<vector<uint64_t> >::iterator bucket_it;
    for (bucket_it = buckets.begin(); bucket_it != buckets.end(); bucket_it++)
    {
        vector<uint64_t>::iterator first, second;
        for (first = bucket_it->begin(); first != bucket_it->end(); ++first)
            for (second = first + 1; second != bucket_it->end(); ++second)
            {
                if (hamming_dist(*first, *second) <= 3)
                {
                    vector<int>::iterator id_it;
                    for (id_it = g_simhash_to_id[*first].begin(); id_it != g_simhash_to_id[*first].end(); id_it++)
                    {
                        if (id_it != g_simhash_to_id[*first].begin())
                            to_drop.push_back(*id_it);
                        //cout << *id_it << " ";
                    }
                    for (id_it = g_simhash_to_id[*second].begin(); id_it != g_simhash_to_id[*second].end(); id_it++)
                    {
                        to_drop.push_back(*id_it);
                        //cout << *id_it << " ";
                    }
                    //cout << endl;
                }
            }
    }
    sort(to_drop.begin(), to_drop.end());
    vector<int>::iterator id_it = unique(to_drop.begin(), to_drop.end());
    to_drop.resize( id_it - to_drop.begin() );
    return to_drop;
}

void add_to_maps(int id, uint64_t hash)
{
    g_id_to_simhash[id] = hash;
    g_simhash_to_id[hash].push_back(id);
}

int main(int argc, char **argv)
{
    Doc* doc = new Doc();
    FILE* input = fopen(argv[1], "r");
	while (get_doc(input, doc)>0)
	{
        uint64_t simhash = charikar_hash64_wide(doc->text.c_str(), doc->text.size(), 3);
        add_to_maps(doc->id, simhash);
        delete doc;
        doc = new Doc();
	}
    if (doc != NULL)
        delete doc;

    vector<int> to_drop = search_duplicates();

    //search_dup_dummy();
    
    fclose(input);
    input = fopen(argv[1], "r");
    vector<int>::iterator id_it = to_drop.begin();
    doc = new Doc();
    while (get_doc(input, doc) > 0)
    {
        if (doc->id == *id_it)
            ++id_it;
        else
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

