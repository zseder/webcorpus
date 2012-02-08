import re
from operator import itemgetter

charset_pattern = re.compile(r"charset=([a-zA-Z0-9\-]+)")
position_interval_pattern = re.compile(r"position ([0-9]*)-([0-9]*)")
position_pattern = re.compile(r"position ([0-9]*):")

_possible_encodings = ["utf-8", "LATIN1", "LATIN2", "windows-1250", "windows-1252"]

def detect_encoding(t):
    s = charset_pattern.search(t)
    if s is not None:
        return s.group(1)
    return None

def guess_encoding(t, possible_encodings=_possible_encodings):
    d = {}
    for pe in possible_encodings:
        value = test_encoding(t, pe)
        if len(d) > 0:
            m = min(d.values())
            if value < m:
                d.clear()
                d[pe] = value
            elif value == m:
                d[pe] = value
        else:
            d[pe] = value
    if len(d) == 1:
        return d.keys()[0]
    else:
        pos = [(k,possible_encodings.index(k)) for k in d.keys()]
        pos.sort(key=itemgetter(1))
        return pos[0][0]

def test_encoding(t, enc):
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

def decode_text(text):
    enc = detect_encoding(text)
    if enc is not None:
        res = test_encoding(text, enc)
        if res > 10:
            guessed = guess_encoding(text)
            enc = guessed
    else:
        enc = guess_encoding(text)
    decoded_text = text.decode(enc, "ignore")
    return decoded_text

