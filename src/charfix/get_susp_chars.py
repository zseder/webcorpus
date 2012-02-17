import sys
from collections import defaultdict

def read_ngram_file(f):
    d = {}
    for l in f:
        l = l.decode("utf-8").rstrip()
        le = l.split("\t")
        if len(le) != 2:
            continue
        ngr, count_s = le
        d[ngr] = int(count_s)
    s = float(sum(d.values()))
    return dict((k, v/s) for k,v in d.items())

def add_possible_missing(d, error_interval=(1,2)):
    d2 = defaultdict(float)
    for ngram in d:
        for ch_len in error_interval:
            for i in xrange(len(ngram)):
                d2[tuple(ngram[:i]) + (None,) + tuple(ngram[i+ch_len:])] += d[ngram]
        d2[tuple(ngram)] = d[ngram]
    return d2

def suspicious(wrong, gold, ratio=10.0, error_interval=(1,2)):
    l = len(wrong)
    pr = 0
    for ngram in wrong:
        if None in ngram:
            continue
        pr += 1
        if (pr * 100) / l > (pr - 1) * 100 / l:
            sys.stderr.write(".")
        
        if ngram not in gold or wrong[ngram] / gold[ngram] > ratio:# or wrong[ngram] / gold[ngram] < 1 / ratio:
            for ch_len in error_interval:
                for ch_index in xrange(len(ngram) - ch_len + 1):
                    maybe_good_ngram = tuple(ngram[:ch_index]) + (None,) + tuple(ngram[ch_index+ch_len:])
                    if maybe_good_ngram in gold:
                        if wrong[maybe_good_ngram] / gold[maybe_good_ngram] < ratio and wrong[maybe_good_ngram] / gold[maybe_good_ngram] > 1 / ratio:
                            yield ngram[ch_index:ch_index+ch_len], wrong[ngram]

def main():
    gold_ngram_f = file(sys.argv[1])
    wrong_ngram_f = file(sys.argv[2])
    gold_dist = read_ngram_file(gold_ngram_f)
    wrong_dist = read_ngram_file(wrong_ngram_f)
    susp = defaultdict(int)
    gold_with_leaveouts = add_possible_missing(gold_dist, (1,2))
    wrong_with_leaveouts = add_possible_missing(wrong_dist, (1,2))
    for chars, freq in suspicious(wrong_with_leaveouts, gold_with_leaveouts):
        susp[chars] += freq
    for ch in susp:
        print "".join(ch).encode("utf-8") + "\t" + str(susp[ch])

if __name__ == "__main__":
    main()
