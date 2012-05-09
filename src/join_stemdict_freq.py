""" This script reads a stemmed dictionary (first argument) and a
frequency list for words (second argument, see freq.py) and
joins them into one file
"""
import sys
from collections import defaultdict

def read_stemmed(f):
    d = defaultdict(set)
    for l in f:
        l = l.strip()
        le = l.split()
        word = le[0]
        stem = le[1]
        d[word].add(stem)
    return d

def read_freq_vocab(f):
    d = {}
    for l in f:
        ls = l.strip()
        le = ls.split("\t", 1)
        try:
            freq = int(le[0])
            word = le[1]
        except IndexError:
            continue
        d[word] = freq
    return d


def main():
    stemmed = read_stemmed(file(sys.argv[1]))
    vocab = read_freq_vocab(file(sys.argv[2]))
    for word in vocab:
        if word in stemmed:
            print "{0}\t{1}\t{2}".format(word, "|".join(stemmed[word]), vocab[word])
        else:
            print "{0}\t{1}\t{2}".format(word, "UNKNOWN_WORD", vocab[word])

if __name__ == "__main__":
    main()
