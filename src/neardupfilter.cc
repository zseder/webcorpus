#include <sstream>
#include <bitset>
#include <map>
#include <vector>
#include <algorithm>
#include <stdint.h>
#include <cstdlib>

#include "reader.h"

typedef map<int, uint64_t> IdSimhashMap;
typedef map<uint64_t, vector<int> > SimhashIdMap;

extern "C" {
    uint64_t charikar_hash64_wide( const char *data, long data_length, int stoken_size);
    int hamming_dist( uint64_t a1, uint64_t a2);
}

#define MIN(a,b) a<b?a:b
#define MAX(a,b) a>b?a:b
#define ABS(a) a>=0?a:-a
#define HAMMING_32(a,b) bit_count(a^b)

using namespace std;

IdSimhashMap g_id_to_simhash;
SimhashIdMap g_simhash_to_id;

// variables and functions for fast bit counting:
static char bits_in_16bits [0x1u << 16];
int iterated_bitcount (unsigned int n)
{
    int count=0;
    while (n)
    {
        count += n & 0x1u ;
        n >>= 1 ;
    }
    return count ;
}
void compute_bits_in_16bits (void)
{
    unsigned int i ;
    for (i = 0; i < (0x1u<<16); i++)
        bits_in_16bits [i] = iterated_bitcount (i) ;
    return ;
}

int bit_count (uint32_t n)
{
    return bits_in_16bits[n & 0xffffu] + bits_in_16bits[(n >> 16) & 0xffffu];
}
// end of bitcount section

vector<int> search_duplicates_with_bitsum_bucketing()
{
    vector<uint64_t> buckets[33][33];
    SimhashIdMap::iterator hash_it; 
    for (hash_it = g_simhash_to_id.begin(); hash_it != g_simhash_to_id.end(); ++hash_it)
    {
        int lower = bits_in_16bits[hash_it->first & 0xffffu] + bits_in_16bits[(hash_it->first >> 16) & 0xffffu];
        int upper = bits_in_16bits[(hash_it->first >> 32) & 0xffffu] + bits_in_16bits[(hash_it->first >> 48) & 0xffffu];
        buckets[lower][upper].push_back(hash_it->first);
    }

    vector<int> to_drop;
    int i = 0;
    time_t progress = time(NULL);
    for (hash_it = g_simhash_to_id.begin(); hash_it != g_simhash_to_id.end(); ++hash_it)
    {
        i++;
        if (i*100/g_simhash_to_id.size() > (i-1)*100/g_simhash_to_id.size())
        {
            cerr << (i-1)*100/g_simhash_to_id.size() << " done in " << time(NULL) - progress << " seconds\n";
            progress = time(NULL);
        }
        int lower = bits_in_16bits[hash_it->first & 0xffffu] + bits_in_16bits[(hash_it->first >> 16) & 0xffffu];
        int upper = bits_in_16bits[(hash_it->first >> 32) & 0xffffu] + bits_in_16bits[(hash_it->first >> 48) & 0xffffu];
        for (int possible_lower = MAX(0, lower - 3); possible_lower != (MIN(33, lower + 4)); possible_lower++)
        {
            for (int possible_upper = MAX(0, upper - 3); possible_upper != (MIN(33, upper + 4)); possible_upper++)
            {
                if (ABS(possible_lower - lower) + ABS(possible_upper - upper) > 3)
                    continue;

                vector<uint64_t>& bucket = buckets[possible_lower][possible_upper];
                vector<uint64_t>::iterator other_it;
                for (other_it = bucket.begin(); other_it != bucket.end(); ++other_it)
                {
                    if (*other_it == hash_it->first)
                        continue;
                    
                    if (hamming_dist(hash_it->first, *other_it) <= 3)
                    {
                        vector<int>::iterator id_it;
                        for (id_it = g_simhash_to_id[*other_it].begin(); id_it != g_simhash_to_id[*other_it].end(); id_it++)
                        {
                            to_drop.push_back(*id_it);
                        }
                    }
                }
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
    // init of bitcount array
    compute_bits_in_16bits();
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

    vector<int> to_drop = search_duplicates_with_bitsum_bucketing();

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

