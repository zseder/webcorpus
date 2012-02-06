import re
import sys

separator = re.compile("<!-- DOCID:([^ ]*) .*-->$")
try:
    unique_sep = file("splitcode.h").read().split("\n")[0].split("\"")[1]
except IOError, e:
    sys.stderr.write("splitcode.h (that contains unique split code and is output of makesplitcode.sh) not found.\n")
    raise e

def main():
    prev_id = None
    for line in sys.stdin:
        m = separator.match(line)
        if m is not None:
            if prev_id is not None:
                print "DOCEND {0} {1}".format(unique_sep, prev_id)
            prev_id = m.group(1).strip()
            print "DOCSTART {0} {1}".format(unique_sep, prev_id)
        else:
            sys.stdout.write(line)

if __name__ == "__main__":
    main()
