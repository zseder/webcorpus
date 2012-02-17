"""
HTML unescaping

HTMLParser.unescape() is an undocumented method but still useful
if we only used htmlentitydefs.name2codepoint, we would not be
able to escape numerical entities like &#822;

xml.sax.saxutils.unescape() method also does less
"""
import sys
import re

from HTMLParser import HTMLParser as HP
p = HP()
for l in sys.stdin:
    l = l.decode("utf-8", "replace")
    while True:
        try:
            l = p.unescape(l)
            break
        except ValueError, e:
            s = str(e)
            if s.startswith("invalid literal for int()"):
                before = l
                l = re.sub("&#?x?" + s.split("'")[1] + ";", "", before)
                if l == before:
                    raise e
            else:
                raise e
    sys.stdout.write(l.encode("utf-8"))

