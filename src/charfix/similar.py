import sys

class Universe:
    """
    hack class that acts like universe set
    (useful when reading gold dict and everything is interesting)
    and acts like a dict to self
    (useful because while reading gold dict, everythin is
     starter for only itself, but when reading only interesting
     characters and they can have more than one length, they are
     detected by their first characters, and their following chars
     are stored in a dict)
    """
    def __contains__(self, item):
        return True
    def __getitem__(self, key):
        return [key]

def read_ngram_file(f, interesting=None):
    my_interesting = {}
    if interesting is not None:
        for inter in interesting:
            if inter[0] not in my_interesting:
                my_interesting[inter[0]] = set()
            my_interesting[inter[0]].add(inter)
        interesting = my_interesting
    else:
        interesting = Universe()

    d = {}
    for l in f:
        le = l.decode("utf-8").strip().split("\t")
        try:
            ngram = le[0]
            fr = int(le[1])
        except Exception:
            continue
        for _index in xrange(len(ngram)):
            char = ngram[_index]
            if interesting is not None and char not in interesting:
                continue
            for possible_char in interesting[char]:
                prev = ngram[:_index]
                next_ = ngram[_index+len(possible_char):]
                if possible_char not in d:
                    d[possible_char] = {}
                d[possible_char][(prev,next_)] = fr
    return d

def similar(wrong_ngrams, gold_ngrams):
    bests = {}
    for w_char in wrong_ngrams:
        best = None
        s = float(sum(wrong_ngrams[w_char].values()))
        for g_char in gold_ngrams:
            # we dont care about whitespaces
            if g_char == "_":
                continue

            # we dont care about not-frequent characters
            gold_s = sum(gold_ngrams[g_char].values())
            if gold_s < s / 100.0:
                continue

            similarity = 0.0
            for env in wrong_ngrams[w_char]:
                try:
                    similarity += gold_ngrams[g_char][env] * ( wrong_ngrams[w_char][env] / s)
                except KeyError:
                    pass
            similarity = similarity / gold_s
            print w_char.encode("utf-8"), g_char.encode("utf-8"), similarity
            if best is None:
                best = (g_char, similarity)
            else:
                if similarity > best[1]:
                    best = (g_char, similarity)
        if best is not None:
            bests[w_char] = best[0]
    return bests

def main():
    gold_f = file(sys.argv[1])
    wrong_f = file(sys.argv[2])
    susp_f = file(sys.argv[3])
    susp = set(l.decode("utf-8").strip().split()[0] for l in susp_f)
    gold_ngrams = read_ngram_file(gold_f)
    wrong_ngrams = read_ngram_file(wrong_f, susp)
    sugg =  similar(wrong_ngrams, gold_ngrams)
    for wc in sugg:
        print wc.encode("utf-8") + "\t" + sugg[wc].encode("utf-8")

if __name__ == "__main__":
    main()
