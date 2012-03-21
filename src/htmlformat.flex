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
 * html formatting script
 * removes empty lines, and multiple whitespaces
**/

%option noyywrap
%s CONTENT PRECONTENT
    #include "splitcode.h"
    /* whitespaces: space, tab, \240 */
SPACE [ 	\240]
NEWLINE [\n\f\v]
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
		printf("\n%s\n",yytext);
		BEGIN(PRECONTENT);
}

    /* removing any whitespaces at start */
<PRECONTENT>({SPACE}|"\n")* {
		BEGIN(CONTENT);
}

    /* removing empty paragraphs from file start */
<PRECONTENT>(<p>|{SPACE}|\n)*<p>(<p>|{SPACE}|\n)* {
		BEGIN(CONTENT);
}


"DOCEND "[0-9]+" "[0-9]+"\n" {		
		if (strncmp(yytext, SPLITCODE, SPLITCODELEN) == 0) {

			BEGIN(INITIAL);
			printf("\n");
		}
		printf("\n%s\n",yytext);
}

    /* replacing more whitespaces with one */
<CONTENT>{SPACE}{SPACE}+ {
		printf(" ");
}

    /* removing empty lines */
<CONTENT>"\n"({SPACE}|\n)+ {
		printf("\n");
}

    /* add empty lines instead of paragraphs (<p>) */
<CONTENT>(<p>|{SPACE}|\n)*"<p>"(<p>|{SPACE}|\n)* {
		printf("\n\n");
}

    /* clean up paragraph boundaries: only one empty line after every */
    /* paragraph and whitespaces at line start are removed            */
<CONTENT>{SPACE}*{NEWLINE}({SPACE}*{NEWLINE})+{SPACE}* {
		printf("\n\n");
	}

    /* remove whitespaces before and after newline */
<CONTENT>{SPACE}*{NEWLINE}{SPACE}* {
		printf("\n");
	}

    /* spaces at line beginning removed */
<CONTENT>^{SPACE}*

    /* multiple spaces replaced by one */
<CONTENT>{SPACE}* {
		printf(" ");
	}
