import sys
import re
from encodings.aliases import aliases
from file_coding_utils import *

charset_pattern = re.compile(r"<meta[^>]*charset=([a-zA-Z\-0-9\"\']*)")
available_encodings = set(aliases.keys() + aliases.values()) 

def get_best_detected(doc):
    enc = detect_encoding(doc)
    guessed_enc = guess_encoding(doc)
    guessed_result = test_encoding(doc, guessed_enc)
    if enc is not None:
        enc_result = test_encoding(doc, enc)
        if guessed_result < enc_result:
            return doc.decode(guessed_enc, "replace")
        else:
            return doc.decode(enc, "replace")
    else:
        return doc.decode(guessed_enc, "replace")


def mydecode(docid, doc):
    if docid is not None:
        search_obj = charset_pattern.search(doc)

        utf_result = test_encoding(doc, "utf-8")
        if utf_result < 5:
            decoded = doc.decode("utf-8", "replace")
        else:
            enc_in_file = None
            if search_obj is not None:
                enc_in_file = search_obj.group(1).strip("\"").strip("'")
                if enc_in_file is not None and enc_in_file in available_encodings and test_encoding(doc, enc_in_file) == 0:
                    decoded = doc.decode(enc_in_file)
                else:
                    decoded = get_best_detected(doc)
            else:
                decoded = get_best_detected(doc)
        return decoded

def read(f):
    id_pattern = re.compile(r"<!-- DOCID:([^ ]*) SIZE:.* -->$")
    # remove any invalid utf-8 characters
    re_pattern = re.compile(u'[\uFFFF]', re.UNICODE)
    prevdocid = None
    prevdoc = ""
    for l in f:
        l = l.strip()
        m = id_pattern.match(l)
        if m is not None:
            if prevdocid is not None:
                decoded = mydecode(prevdocid, prevdoc)
                print prevdocid
                print re_pattern.sub(u'\uFFFD', decoded.encode("utf-8").strip())
            prevdocid = m.group(0)
            prevdoc = ""
        else:
            prevdoc += l + "\n"

    # handling last doc
    decoded = mydecode(prevdocid, prevdoc)
    print prevdocid
    print decoded.encode("utf-8").strip()
    print re_pattern.sub(u'\uFFFD', decoded.encode("utf-8").strip())

def main():
    read(sys.stdin)

if __name__ == "__main__":
    main()

