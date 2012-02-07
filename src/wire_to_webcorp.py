import re
import sys

from splitcode import SPLITCODE

separator = re.compile("<!-- DOCID:([^ ]*) .*-->$")

def main():
    prev_id = None
    for line in sys.stdin:
        m = separator.match(line)
        if m is not None:
            if prev_id is not None:
                print "DOCEND {0} {1}".format(SPLITCODE, prev_id)
            prev_id = m.group(1).strip()
            print "DOCSTART {0} {1}".format(SPLITCODE, prev_id)
        else:
            sys.stdout.write(line)

if __name__ == "__main__":
    main()
