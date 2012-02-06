%option noyywrap
%s CONTENT PRECONTENT
	#include "splitcode.h"
SPACE [ 	\240]
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
	// kezd�dik a becsomagolt �llom�ny
		printf("%s",yytext);
		BEGIN(PRECONTENT);
	}

<PRECONTENT>({SPACE}|"\n")* {
		// �llom�ny elej�n l�v� sz�k�z�k �s �res sorok t�rl�se
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

<CONTENT>{SPACE}{SPACE}+ {
		// sorban szerepl� t�bb sz�k�z egyre cser�l�se
		printf(" ");
	}

<CONTENT>"\n"({SPACE}|\n)+ {
		// �res sorok t�rl�se
		printf("\n");
	}

<CONTENT>(<p>|{SPACE}|\n)*"<p>"(<p>|{SPACE}|\n)* {
		// �res bekezd�sek t�rl�se
		printf("\n\n");
	}

<PRECONTENT>(<p>|{SPACE}|\n)*<p>(<p>|{SPACE}|\n)* {
		// �llom�ny elej�n l�v� �res bekezd�sek t�rl�se
		BEGIN(CONTENT);
	}

<CONTENT>"<p>" {
		// bekezd�s �res sorr� alak�t�sa
		printf("\n\n");
	}
