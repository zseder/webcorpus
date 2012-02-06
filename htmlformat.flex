%option noyywrap
%s CONTENT PRECONTENT
	#include "splitcode.h"
SPACE [ 	\240]
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {		
	// kezdõdik a becsomagolt állomány
		printf("%s",yytext);
		BEGIN(PRECONTENT);
	}

<PRECONTENT>({SPACE}|"\n")* {
		// állomány elején lévõ szóközök és üres sorok törlése
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

<CONTENT>{SPACE}{SPACE}+ {
		// sorban szereplõ több szóköz egyre cserélése
		printf(" ");
	}

<CONTENT>"\n"({SPACE}|\n)+ {
		// üres sorok törlése
		printf("\n");
	}

<CONTENT>(<p>|{SPACE}|\n)*"<p>"(<p>|{SPACE}|\n)* {
		// üres bekezdések törlése
		printf("\n\n");
	}

<PRECONTENT>(<p>|{SPACE}|\n)*<p>(<p>|{SPACE}|\n)* {
		// állomány elején lévõ üres bekezdések törlése
		BEGIN(CONTENT);
	}

<CONTENT>"<p>" {
		// bekezdés üres sorrá alakítása
		printf("\n\n");
	}
