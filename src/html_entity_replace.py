import sys
import HTMLParser

p = HTMLParser()

for l in sys.stdin:
    l = p.unescape(l.decode("utf-8"))
    sys.stdout.write(l.encode("utf-8"))

