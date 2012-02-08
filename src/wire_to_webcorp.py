"""
This script changes wire-like document separators with the ones
we use in all our scripts
Reads from stdin and writes to stdout
"""
import re
import sys

from splitcode import header, footer

separator = re.compile("<!-- DOCID:([^ ]*) .*-->$")

def main():
    prev_id = None
    for line in sys.stdin:
        m = separator.match(line)
        if m is not None:
            if prev_id is not None:
                print "{0} {1}".format(header, prev_id)
            prev_id = m.group(1).strip()
            print "{0} {1}".format(footer, prev_id)
        else:
            sys.stdout.write(line)

if __name__ == "__main__":
    main()
