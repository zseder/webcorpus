import sys
from collections import defaultdict

from splitcode import header, footer

def main():
    paragraphs = defaultdict(int)
    bound = int(sys.argv[1])
    doc = ""
    for l in sys.stdin:
        sys.stdout.write(l)
        if l.startswith(header):
            if len(doc) > 0:
                new_doc = []
                myparagraphs = doc.split("<p>")
                for myp in myparagraphs:
                    stripped = myp.strip()
                    paragraphs[stripped] += 1
                    if paragraphs[stripped] < bound:
                        new_doc.append(myp)
                sys.stdout.write("<p>".join(new_doc))
            doc = ""
        elif l.startswith(footer):
            sys.stdout.write(l)
        else:
            doc += l

if __name__ == "__main__":
    main()
