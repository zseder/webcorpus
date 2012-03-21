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
                print "\n{0} {1}".format(footer, prev_id)
            prev_id = m.group(1).strip()
            print "{0} {1}".format(header, prev_id)
        else:
            sys.stdout.write(line)

if __name__ == "__main__":
    main()
