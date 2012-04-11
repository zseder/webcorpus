"""
Copyright 2012 Judit Acs
Email: judit@sch.bme.hu

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

# this script converts heritrix web crawler output (ARC) to
# wire output
# the webcorpus pipeline uses wire archives

# python library includes
import re
import sys
import os
import random
import string

# project includes
SRC_DIR="/home/judit/webcorpus/bin"
sys.path.append(SRC_DIR)
import splitcode

# regex to recognize new documents in heritrix output
NEWDOCREGEX = "^http.*(\d{1,3}\.){3}\d{1,3}\ \d{14}"

# wire document starting and ending lines
DOCSTART = "DOCSTART "+splitcode.SPLITCODE+" 1\n"
DOCEND = "DOCEND "+splitcode.SPLITCODE+" 1\n"

# supported MIME types
# text/xhtml was left out because less than 1% of our corpus had that type
supported_types = ["text/html", "text/plain"]

# finite state machine states used as enum
class state:
    start = 1
    firstline = 2
    properdoc = 3
    garbage = 4
    end = 5

doc = []

def main():
    doc = []
    s = state.start
    for line in sys.stdin:
        # start state, stays here until new document beginning is found
        if s == state.start:
            match = re.match(str(NEWDOCREGEX), line)
            if match:
                doc = []
                mimetype = line.split(' ')[-2]
                if mimetype.lower() in supported_types:
                    s = state.firstline
                else:
                    s = state.garbage
        # a new document was detected on the previous line
        # gets HTTP response code
        elif s == state.firstline:
            http_code = line.split(' ')[1]
            if http_code == "200":
                s = state.properdoc
            else:
                s = state.garbage
        # Not found or unsupported MIME type (i.e. images)
        elif s == state.garbage:
            match = re.match(str(NEWDOCREGEX), line)
            if match:
                doc = []
                mimetype = line.split(' ')[-2]
                if mimetype.lower() in supported_types:
                    s = state.firstline
                else:
                    s = state.garbage
        # document to be saved
        elif s == state.properdoc:
            match = re.match(str(NEWDOCREGEX), line)
            if match:
            # saving document with wire markup
                print DOCSTART
                print ''.join(doc)
                print DOCEND
                doc = []
                mimetype = line.split(' ')[-2]
                if mimetype.lower() in supported_types:
                    s = state.firstline
                else:
                    s = state.garbage
            else:
                doc.append(line)
    # saving last document of the file
    if s == state.properdoc:
        print DOCSTART
        print ''.join(doc)
        print DOCEND
                
if __name__ == "__main__":
    main()

