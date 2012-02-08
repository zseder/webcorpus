/**
 * html formatting script
 * removes empty lines, and multiple whitespaces
**/

%option noyywrap
%s CONTENT PRECONTENT
    #include "splitcode.h"
    /* whitespaces: space, tab, \240 */
SPACE [ 	\240]
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
		printf("%s",yytext);
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

"DOCEND "[0-9]+"\n" {		
		if (strncmp(yytext, SPLITCODE, SPLITCODELEN) == 0) {
			BEGIN(INITIAL);
			printf("\n");
		}
		printf("%s",yytext);
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
