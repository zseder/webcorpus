/**
 * Removes html tags from html file
**/
%option noyywrap
%s CONTENT
%s SCRIPT STYLE CODE SAMP KBD VAR LISTING ADDRESS PRE COMMENT TAG TITLE
/* pre, address is blokkszer�en viselkedik, de ezek tartalm�t t�r�lj�k is */
/* br, img, option �s li blokkelemszer�ek, ez�rt ezeket felvessz�k */
BLOCK ("br"|"img"|"option"|"li"|"p"|"ul"|"ol"|"h"[1-6]|"dir"|"menu"|"div"|"center"|"noscript"|"noframes"|"blockquote"|"form"|"isindex"|"hr"|"table"|"fieldset"|"th"|"td")
CHARSTYLE ("i"|"b"|"tt"|"strike"|"s"|"u"|"big"|"small"|"font"|"span"|"em"|"strong"|"cite"|"dfn"|"abbr"|"acronym")
	#include "splitcode.h"
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
// kezd�dik a becsomagolt �llom�ny
		printf("%s",yytext);
		BEGIN(CONTENT);
	}

"DOCEND "[0-9]+"\n" {		
	// v�get �r a becsomagolt �llom�ny, ha j� a SPLITCODE
		if (strncmp(yytext, SPLITCODE, SPLITCODELEN) == 0) {
			BEGIN(INITIAL);
			printf("\n");
		}
		printf("%s",yytext);
	}

<CONTENT>"<""/"?{BLOCK}(" "[^>]*)?">" {
		// bekezd�shat�r a blokkelemekn�l
		printf("<p>");
	}
	
		/* karakterst�lus elemek c�mk�inek t�rl�se (sz�k�z nem marad ut�nuk) */
<CONTENT>"<""/"?{CHARSTYLE}(" "[^>]*)?">"

		/* rendszerint nem sz�veget tartalmaz� elemek t�rl�se */
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

		/* egy�b c�mk�k t�rl�se, egy sz�k�z meghagy�s�val */
<CONTENT>"<" { BEGIN(TAG); }

<TAG>[a-zA-Z]*">" {
		BEGIN(CONTENT);
		printf(" ");
	}

<SCRIPT,STYLE,CODE,SAMP,KBD,VAR,LISTING,ADDRESS,PRE,TAG,TITLE>.|"\n"

<CONTENT>"\r" {
		// kocsivissza karakter �jsor karakterr� alak�t�sa
		printf("\n");
}

<CONTENT>">" {
		// > alak�t�sa &gt;-v�
		printf("&gt;");
}
