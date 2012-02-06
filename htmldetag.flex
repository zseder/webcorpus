/**
 * Removes html tags from html file
**/
%option noyywrap
%s CONTENT
%s SCRIPT STYLE CODE SAMP KBD VAR LISTING ADDRESS PRE COMMENT TAG TITLE
/* pre, address is blokkszerûen viselkedik, de ezek tartalmát töröljük is */
/* br, img, option és li blokkelemszerûek, ezért ezeket felvesszük */
BLOCK ("br"|"img"|"option"|"li"|"p"|"ul"|"ol"|"h"[1-6]|"dir"|"menu"|"div"|"center"|"noscript"|"noframes"|"blockquote"|"form"|"isindex"|"hr"|"table"|"fieldset"|"th"|"td")
CHARSTYLE ("i"|"b"|"tt"|"strike"|"s"|"u"|"big"|"small"|"font"|"span"|"em"|"strong"|"cite"|"dfn"|"abbr"|"acronym")
	#include "splitcode.h"
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
// kezdõdik a becsomagolt állomány
		printf("%s",yytext);
		BEGIN(CONTENT);
	}

"DOCEND "[0-9]+"\n" {		
	// véget ér a becsomagolt állomány, ha jó a SPLITCODE
		if (strncmp(yytext, SPLITCODE, SPLITCODELEN) == 0) {
			BEGIN(INITIAL);
			printf("\n");
		}
		printf("%s",yytext);
	}

<CONTENT>"<""/"?{BLOCK}(" "[^>]*)?">" {
		// bekezdéshatár a blokkelemeknél
		printf("<p>");
	}
	
		/* karakterstílus elemek címkéinek törlése (szóköz nem marad utánuk) */
<CONTENT>"<""/"?{CHARSTYLE}(" "[^>]*)?">"

		/* rendszerint nem szöveget tartalmazó elemek törlése */
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

		/* egyéb címkék törlése, egy szóköz meghagyásával */
<CONTENT>"<" { BEGIN(TAG); }

<TAG>[a-zA-Z]*">" {
		BEGIN(CONTENT);
		printf(" ");
	}

<SCRIPT,STYLE,CODE,SAMP,KBD,VAR,LISTING,ADDRESS,PRE,TAG,TITLE>.|"\n"

<CONTENT>"\r" {
		// kocsivissza karakter újsor karakterré alakítása
		printf("\n");
}

<CONTENT>">" {
		// > alakítása &gt;-vé
		printf("&gt;");
}
