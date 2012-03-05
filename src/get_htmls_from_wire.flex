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
<INITIAL,HTML>"<html>" {
    if (YYSTATE == HTML) {
       enddoc(); 
    }
    startdoc();
    BEGIN(HTML);
}

<INITIAL,HTML>"<head>" {
    if (YYSTATE == HTML) {
       enddoc(); 
    }
    startdoc();
    if (!before_first) {printf("<head>");}
    BEGIN(HTML);
}

<INITIAL,HTML>"<body>" {
    if (YYSTATE == HTML) {
       enddoc(); 
    }
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

<CONTENT>(.|\n) {
    if (!before_first) {printf("%s", yytext);}
}

