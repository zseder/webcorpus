/**
 * Reads WIRE and extracts html data 
 * This script is useful only when data
 * from WIRE cannot be extracted with wire-info-extract
 * because its storage is messed up somehow
**/
%option noyywrap
%x HTML BODY
%{
	#include "splitcode.h"
    int was_html = 0;
    long docid = 0;
%}

%%
<INITIAL>"<body>" {
    docid++;
    printf("DOCSTART %s %d\n", SPLITCODE, docid);
    printf("<html><body>");
    was_html = 0;
    BEGIN(HTML);
}
<INITIAL>"<html>" {
    docid++;
    printf("DOCSTART %s %d\n", SPLITCODE, docid);
    printf("<html>");
    was_html = 1;
    BEGIN(HTML);
}

<INITIAL>"<head>" {
    docid++;
    printf("DOCSTART %s %d\n", SPLITCODE, docid);
    printf("<html><head>");
    was_html = 0;
    BEGIN(HTML);
}

<INITIAL>.

<HTML>"</body>" {
    printf("</body>");
    if(!was_html) {
        printf("</html>\n");
        printf("DOCEND %s %d\n", SPLITCODE, docid);
        BEGIN(0);
    }
}

<HTML>"</html>" {
    if(was_html) {
        printf("</html>\n");
        printf("DOCEND %s\n", SPLITCODE);
        was_html = 0;
        BEGIN(0);
    }
}

    /*<CONTENT>(.|\n) {
        printf("%s", yytext);
    }*/

