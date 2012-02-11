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
	
<CONTENT>"<""/"?{CHARSTYLE}(" "[^>]*)?">"

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
