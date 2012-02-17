import os
import sys
from collections import defaultdict
import logging
import gc
import re

from splitcode import header, footer

def ngrams(sequence, n):
    sequence = list(sequence)
    count = max(0, len(sequence) - n + 1)
    return [tuple(sequence[i:i+n]) for i in xrange(count)]

def main():
    inputfn = sys.argv[1]
    n = int(sys.argv[2])
    fs = os.path.getsize(inputfn)
    processed = 0
    d = defaultdict(int)
    gc.disable()
    hlen = len(header)
    flen = len(footer)
    for l in file(inputfn):
        if l[:hlen] == header or l[:flen] == footer:
            continue
        processed += len(l)
        if processed * 100 / fs > (processed - len(l)) * 100 / fs:
            logging.debug("processed {0} percent".format(processed * 100 / fs))
            gc.collect()
        l = l.decode("utf-8", "ignore")
        l = re.sub("\s", "_", l)
        for ng in ngrams(l, n):
            d[ng] += 1

    for tg in d:
        print u"".join(tg).encode("utf-8") + "\t" + str(d[tg])


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(asctime)s : %(message)s")
    main()
