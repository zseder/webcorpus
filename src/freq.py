"""
Copyright 2012 Attila Zseder
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
"""
"""This script just counts word in a corpus, with a simple whitespace
tokenizing method.
Way faster than doing this with "sed", "sort", "uniq -c" because this pipeline
needs a sort on a very big corpus just to count occurences
"""
import sys
import re
from collections import defaultdict
from splitcode import header, footer

def main():
    p = re.compile("\s")
    d = defaultdict(int)
    for l in sys.stdin:
        if l.startswith(header) or l.startswith(footer):
            continue
        l = l.strip()
        for w in p.split(l):
            if w != "":
                d[w] += 1

    s = sorted(d.iteritems(), key=lambda x: x[1], reverse=True)
    for k, v in s:
        print "{0}\t{1}".format(v, k)

if __name__ == "__main__":
    main()
