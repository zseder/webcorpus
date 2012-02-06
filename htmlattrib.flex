%option noyywrap
%s CONTENT
%s TAG DOUBLEQUO_VALUE SINGLEQUO_VALUE COMMENT ELEMENT

MAGYAR_ACCENTED_LETTERS ([������������������]|"&aacute"|"&eacute"|"&iacute"|"&oacute"|"&ouml"|"&otilde"|"&uacute"|"&uuml"|"&ucirc")

	#include "splitcode.h"
	#define CODE ".---"
	int meta;
	char code[5];
	
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {
		// kezd�dik a becsomagolt �llom�ny
		printf("%s",yytext);
		BEGIN(CONTENT);
		
		// inicializ�ljuk a k�dol�st t�rol� mg a meta elemet jelz� v�ltoz�t 
		strcpy(code, CODE);
		meta = 0;
	}

"DOCEND "[0-9]+"\n" {
		// v�get �r a becsomagolt �llom�ny, ha j� a SPLITCODE
//		if (strncmp(yytext, SPLITCODE, SPLITCODELEN) == 0) {
			BEGIN(INITIAL);
			printf("\n%s",yytext);
//			printf("\n<name>\n%s\n</name>", code);
//		} else {
//			printf ("IDE");
//			ECHO;
//		}
	}

<CONTENT>"<!--" {
		// HTML megjegyz�s kezd�dik
		BEGIN(COMMENT);
	}

<COMMENT>"-->" {
		// HTML megjegyz�s v�ge
		BEGIN(CONTENT);
	}

		/* HTML megjegyz�sek tartalm�nak t�rl�se */
<COMMENT>.|\n


<CONTENT>"<META" {
		// META c�mke kezdete
		meta = 1;
		printf("%s",yytext);
		BEGIN(ELEMENT);
	}


<CONTENT>"<"[/]* {
		// HTML c�mke kezdete
		meta = 0;
		printf("%s",yytext);
		BEGIN(ELEMENT);
	}

<ELEMENT>">" {
		// HTML c�mke v�ge
		printf("%s",yytext);
		BEGIN(CONTENT);
	}
<ELEMENT>[^>a-zA-Z] {
		// HTML c�mk�n bel�l, elemn�ven t�l
		BEGIN(TAG);
	}

<TAG>">" {
		// HTML c�mke v�ge
		printf("%s",yytext);
		BEGIN(CONTENT);
	}

		/* T�rl�nk mindent a c�mk�b�l az elemn�ven k�v�l */
<TAG>[^>]

		/* attrib�tumokban lehet < �s > jel, ez is lekezelve */
<TAG>"=\"" { BEGIN(DOUBLEQUO_VALUE); }
	
<TAG>"='" { BEGIN(SINGLEQUO_VALUE); }

<DOUBLEQUO_VALUE>"8859-1" {
	if (meta) code[1] = '1';
}

<DOUBLEQUO_VALUE>"8859-2" {
	if (meta) code[1] = '2';
}

<DOUBLEQUO_VALUE>"1250" {
	if (meta) {
		code[1] = '2';
		code[2] = '5';
	}
}

<DOUBLEQUO_VALUE>"1252" {
	if (meta) {
		code[1] = '1';
		code[2] = '5';
	}
}

<DOUBLEQUO_VALUE>"utf-8" {
	if (meta) code[1] = 'u';
}

<DOUBLEQUO_VALUE>"\"" { BEGIN(TAG); }
<DOUBLEQUO_VALUE>[^\"]

<SINGLEQUO_VALUE>"8859-1" {
	if (meta) code[1] = '1';
}

<SINGLEQUO_VALUE>"8859-2" {
	if (meta) code[1] = '2';
}

<SINGLEQUO_VALUE>"1250" {
	if (meta) {
		code[1] = '2';
		code[2] = '5';
	}
}

<SINGLEQUO_VALUE>"1252" {
	if (meta) {
		code[1] = '1';
		code[2] = '5';
	}
}

<SINGLEQUO_VALUE>"utf-8" {
	if (meta) code[1] = 'u';
}


<SINGLEQUO_VALUE>"'" { BEGIN(TAG); }
<SINGLEQUO_VALUE>[^']

<CONTENT>"&#"[0-9]{1,5}";"? {
		// decim�lis k�d alapj�n Windows-125x felismer�se
		int i;
		char * p = yytext + strlen(yytext);
		if (*p == ';') *p = '\0';
		i = atoi(yytext + 2);
		if (i <= 127) {
			printf("%c", i);	// ASCII
		} else if ((i >= 200) && (i <= 237)) {
			if ((code[2] == '-') && (code[1] != 'u')) {
				code[2] = 'w';
			}
			ECHO;
		} else {
			ECHO;
		}
}

	    /* nem t�r� sz�k�z�k �talak�t�sa */
	    
<CONTENT>"&nbsp"";"? {
		printf(" ");
}

	    /* hosszabb k�ddal nem foglalkozunk */
	    
<CONTENT>"&#"[0-9]*";"? {
		ECHO;
}

	    /* karakter alapj�n Windows-125x felismer�se */

<CONTENT>{MAGYAR_ACCENTED_LETTERS} {
	ECHO;
	code[3] = 'm';
	
}

<CONTENT>[\200-\237] {
	ECHO;
			if ((code[2] == '-') && (code[1] != 'u')) {
				code[2] = 'w';
			}
}

	/* magyar �kezetes Unicode karakterek felismer�se */ 

<CONTENT>("ö"|"ü"|"ó"|"ő"|"ú"|"é"|"á"|"ű"|"í"|"Ö"|"Ü"|"Ó"|"Ő"|"Ú"|"É"|"Á"|"Ű"|"Í") {
	ECHO;
	if (code[1] == 'u') code[3] = 'm';
}
