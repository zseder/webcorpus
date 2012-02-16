/**
 * Reads WIRE and extracts everything between <body> and
 * </body> html tags. This script is useful only when data
 * from WIRE cannot be extracted with wire-info-extract
 * because its storage is messed up somehow
**/
%option noyywrap
%x CONTENT
	#include "splitcode.h"
%%
<INITIAL>"<body>" {
    printf("DOCSTART %s\n", SPLITCODE);
    printf("<body>");
    BEGIN(CONTENT);
}

<INITIAL>.

<CONTENT>"</body>" {
    printf("<body>\n");
    printf("DOCEND %s\n", SPLITCODE);
    BEGIN(0);
}

<CONTENT>(.|\n) {
    printf("%s", yytext);
}

