/*
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
*/

/**
 * Simple filter that throws away all documents that doesn't have at least MIN_LINES
 * sentences
 * */
#include <string>
#include "reader.h"
#define MIN_LINES	4 

using namespace std;

int main(int argc, char **argv)
{
    int min_lines = argc > 1 ? atoi(argv[1]) : MIN_LINES;
    Doc* doc = new Doc();
	while(get_doc(stdin, doc) > 0)
	{
        int line_num = 0;
        size_t pos = doc->text.find("\n", 0);
        while (pos != string::npos)
        {
            line_num++;
            if (line_num >= min_lines)
            {
                cout << "DOCSTART " << SPLITCODE << " " << doc->id << endl;
                cout << doc->text;
                cout << "DOCEND " << SPLITCODE << " " << doc->id << endl;
                break;
            }
            pos = doc->text.find("\n", pos+1);
        }
        delete doc;
        doc = new Doc();
	}
	return 0;
}
