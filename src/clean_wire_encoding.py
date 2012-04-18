"""
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
"""

import sys
import re
from encodings.aliases import aliases

from splitcode import header, footer

charset_pattern = re.compile(r"<meta[^>]*charset=([a-zA-Z\-0-9\"\']*)")
available_encodings = set((_.lower() for _ in aliases.keys()))
available_encodings |= set((_.lower() for _ in aliases.values()))
# for detect invalid positions in UnicodeError message
position_interval_pattern = re.compile(r"position ([0-9]*)-([0-9]*)")
position_pattern = re.compile(r"position ([0-9]*):")

def test_encoding(t, enc):
    """
    tests a "t" text decoding with enc and returns how many decode errors
    occured in the whole text
    """
    c = 0
    while True:
        try:
            t = t.decode(enc)
            break
        except LookupError:
            return len(t)
        except UnicodeError, e:
            msg = str(e)
            s = position_interval_pattern.search(msg)
            if s is not None:
                start, stop = int(s.group(1)), int(s.group(2))
                t = t[stop+1:]
                c += 1
            else:
                s = position_pattern.search(msg)
                if s is not None:
                    pos = int(s.group(1))
                    t = t[pos+1:]
                    c += 1
                else:
                    raise e
    return c

def mydecode(doc):
    """
    decodes doc first with utf-8, but if fails, tries to detect
    encoding and decode doc with that
    """

    utf_result = test_encoding(doc, "utf-8")
    if utf_result < 5:
        decoded = doc.decode("utf-8", "replace")
        return decoded
    else:
        search_obj = charset_pattern.search(doc)
        enc_in_file = None
        if search_obj is not None:
            enc_in_file = search_obj.group(1).strip("\"").strip("'").lower().replace('-','_')
            if enc_in_file is not None and enc_in_file in available_encodings and test_encoding(doc, enc_in_file) == 0:
                decoded = doc.decode(enc_in_file)
                return decoded

def read(f):
    """
    Iterates through all documents and decode them
    """
    # remove any invalid utf-8 characters
    re_pattern = re.compile(u'[\uFFFF]', re.UNICODE)
    mydoc = ""
    myheader = None
    for l in f:
        if l.startswith(header):
            myheader = l.strip()
            mydoc = ""
        elif l.startswith(footer):
            decoded = mydecode(mydoc)
            print myheader
            if decoded is not None:
                print re_pattern.sub(u'\uFFFD', decoded.encode("utf-8").strip())
            else:
                print mydoc
            sys.stdout.write(l)
        else:
            mydoc += l

def main():
    read(sys.stdin)

if __name__ == "__main__":
    main()

