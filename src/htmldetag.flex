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
 * Removes html tags from html file
**/
%option noyywrap
%s CONTENT
%s SCRIPT STYLE CODE SAMP KBD VAR LISTING ADDRESS PRE COMMENT TAG TITLE
/* pre, adress tags work like block but we drop them */
/* br, img, option, li tags act like blocks so we add them */
BLOCK ("br"|"img"|"option"|"li"|"p"|"ul"|"ol"|"h"[1-6]|"dir"|"menu"|"div"|"center"|"noscript"|"noframes"|"blockquote"|"form"|"isindex"|"hr"|"table"|"fieldset"|"th"|"td")
CHARSTYLE ("i"|"b"|"tt"|"strike"|"s"|"u"|"big"|"small"|"font"|"span"|"em"|"strong"|"cite"|"dfn"|"abbr"|"acronym")
	#include "splitcode.h"
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
		printf("%s",yytext);
		BEGIN(CONTENT);
	}

"DOCEND "[0-9]+" "[0-9]+"\n" {		
		if (strncmp(yytext, SPLITCODE, SPLITCODELEN) == 0) {
			BEGIN(INITIAL);
			printf("\n");
		}
		printf("\n%s",yytext);
}

<CONTENT>"<""/"?{BLOCK}(" "[^>]*)?">" {
        /* handle every block as new paragraph */
		printf("<p>");
}
	
<CONTENT>"<""/"?{CHARSTYLE}(" "[^>]*)?">" {
    printf(" ");
}

    /* deleting tags that does not usually contain text */
<CONTENT>"<title>" { BEGIN(TITLE); }
<TITLE>"</title>" { BEGIN(CONTENT); }
<CONTENT>"<script>" { BEGIN(SCRIPT); }
<SCRIPT>"</script>" { BEGIN(CONTENT); }
<CONTENT>"<style" { BEGIN(STYLE); }
<STYLE>"</style>" { BEGIN(CONTENT); }
<CONTENT>"<code>" { BEGIN(CODE); }
<CODE>"</code>" { BEGIN(CONTENT); }
<CONTENT>"<samp>" { BEGIN(SAMP); }
<SAMP>"</samp>" { BEGIN(CONTENT); }
<CONTENT>"<kbd>" { BEGIN(KBD); }
<KBD>"</kbd>" { BEGIN(CONTENT); }
<CONTENT>"<var>" { BEGIN(VAR); }
<VAR>"</var>" { BEGIN(CONTENT); }
<CONTENT>"<listing>" { BEGIN(LISTING); }
<LISTING>"</listing>" { BEGIN(CONTENT); }
<CONTENT>"<address>" { BEGIN(ADDRESS); printf("<p>"); }
<ADDRESS>"</address>" { BEGIN(CONTENT); }
<CONTENT>"<pre>" { BEGIN(PRE); printf("<p>"); }
<PRE>"</pre>" { BEGIN(CONTENT); }

    /* when removing other tags, add a space */
<CONTENT>"<" { BEGIN(TAG); }

<TAG>[a-zA-Z]*">" {
		BEGIN(CONTENT);
		printf(" ");
	}

<SCRIPT,STYLE,CODE,SAMP,KBD,VAR,LISTING,ADDRESS,PRE,TAG,TITLE>.|"\n"

    /* fixing \r endlines */
<CONTENT>"\r" {
		printf("\n");
}

<CONTENT>">" {
		printf("&gt;");
}
