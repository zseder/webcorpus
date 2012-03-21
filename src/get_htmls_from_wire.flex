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
 * Reads WIRE and extracts html data 
 * This script is useful only when data
 * from WIRE cannot be extracted with wire-info-extract
 * because its storage is messed up somehow
**/
%option noyywrap
%x HTML
%{
	#include "splitcode.h"
    long docid = 0;
    int was_body;
    int before_first = 1;

    void enddoc() {
        printf("</html>\n");
        printf("DOCEND %s %d\n", SPLITCODE, docid);
        was_body = 0;
    }
    void startdoc() {
        printf("DOCSTART %s %d\n", SPLITCODE, ++docid);
        printf("<html>");
        before_first = 0;
    }
%}

%%
<INITIAL,HTML>"<html"[^>]*">" {
    if (YYSTATE == HTML) {
       enddoc(); 
    }
    startdoc();
    BEGIN(HTML);
}

<INITIAL>"<head>" {
    startdoc();
    if (!before_first) {printf("<head>");}
    BEGIN(HTML);
}

<INITIAL>"<body>" {
    startdoc();
    if (!before_first) {printf("<body>");}
    was_body = 1;
    BEGIN(HTML);
}

<INITIAL>.

<HTML>"</body>" {
    if (!before_first) {printf("</body>");}
    enddoc();
    BEGIN(0);
}

    /* this is just error handling, </body> should come always first*/
<HTML>"</html>" {
    if(was_body) {
        if (!before_first) {printf("</body>");}
    }
    enddoc();
    BEGIN(0);
}

<HTML>(.|\n) {
    if (!before_first) {printf("%s", yytext);}
}

