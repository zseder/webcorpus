import re
import sys

def read(f):
    patterns = []
    for l in f:
        l = l.decode("utf-8").strip()
        le = l.split("\t")
        patterns.append((re.compile(le[0]), le[1]))
    return patterns

def main():
    patterns = read(file(sys.argv[1]))
    for l in sys.stdin:
        l = l.decode("utf-8", "ignore")
        for p, s in patterns:
            l = p.sub(s, l)
        sys.stdout.write(l.encode("utf-8"))

if __name__ == "__main__":
    main()
