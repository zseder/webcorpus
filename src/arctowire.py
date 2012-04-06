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
import optparse
import random
import string
import gzip

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

# command line options
def setup_parser():
	parser.add_option('--input', dest='infile', help="Single input file's name and path.")
	parser.add_option('--indir', dest='indir', help="Input directory. May contain multiple files.")
	parser.add_option('--output', dest='outfile', help="Name of output file. Multiple output files are not supported.")
	parser.add_option('--mode', choices=['overwrite', 'append'], default='overwrite', help="Overwrite or append to existing output file.")
	parser.add_option('--compressed', choices=['none', 'gzip', 'gz'], default='none', help="Compression type. Currently only gzip and plain text are supported.")
	parser.add_option('--postfix', dest='postfix', default='', help="Output file name postfix. Pretty useless option.")
	parser.add_option('--verbose', dest='verbose', action="store_true", default=False, help="Verbose mode.")
	(options, args) = parser.parse_args()
## Making sure all mandatory options appeared.
#	if options.__dict__["outfile"] == "":
#		print "Mandatory option is missing: output\n"
#        parser.print_help()
#        exit(-1)
#	if options.infile=="" and options.indir=="":
#		print "Input or indir must be specified\n"
#        parser.print_help()
#        exit(-1)
#		
	return options
	

# if directory is specified then read all files in the directory
def get_file_list():
	files = []
	filenames = []
	if options.indir:
		pathroot = options.indir+"/"
		filenames = os.listdir(options.indir)
	for f in filenames:
		files.append(str(pathroot+f))
	# if input option is specified the input file is added as well
	if options.infile:
		files.append(options.infile)
	return files
	
doc = []
parser = optparse.OptionParser()
options = setup_parser()

def open_output_file(path):
	if options.mode=='overwrite':
		file_handler=open(path, "w")
	elif options.mode=='append':
		file_handler=open(path, "a")
	return file_handler

def open_input_file(path):
	if options.compressed=='gzip' or options.compressed=='gz':
		file_handler=gzip.open(path)
	else:
		file_handler=open(path)
	return file_handler
	
def main():
	files = get_file_list()
	out = open_output_file(options.outfile)
	doc = []

	for f in files:
		if options.verbose == True:
			print "filename: " + f
		s = state.start
		current_file = open_input_file(f)
		for line in current_file:
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
					out.write(DOCSTART)
					out.writelines(doc)
					out.write(DOCEND)
					doc = []
					mimetype = line.split(' ')[-2]
					if mimetype.lower() in supported_types:
						s = state.firstline
					else:
						s = state.garbage
				else:
					doc.append(line)
		# saving last document of the file
		out.write(DOCSTART)
		out.writelines(doc)
		out.write(DOCEND)
		current_file.close()				
	out.close()
				
if __name__ == "__main__":
	main()

