%option noyywrap
%s CONTENT
%s TAG DOUBLEQUO_VALUE SINGLEQUO_VALUE COMMENT ELEMENT

MAGYAR_ACCENTED_LETTERS ([ˆ¸Ûı˙È·˚Ì÷‹”’⁄…¡€Õ]|"&aacute"|"&eacute"|"&iacute"|"&oacute"|"&ouml"|"&otilde"|"&uacute"|"&uuml"|"&ucirc")

	#include "splitcode.h"
	#define CODE ".---"
	int meta;
	char code[5];
	
%%
"DOCSTART "[0-9]+" "[0-9]+"\n" {
		// kezdıdik a becsomagolt ·llom·ny
		printf("%s",yytext);
		BEGIN(CONTENT);
		
		// inicializ·ljuk a kÛdol·st t·rolÛ mg a meta elemet jelzı v·ltozÛt 
		strcpy(code, CODE);
		meta = 0;
	}

"DOCEND "[0-9]+"\n" {
		// vÈget Èr a becsomagolt ·llom·ny, ha jÛ a SPLITCODE
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
		// HTML megjegyzÈs kezdıdik
		BEGIN(COMMENT);
	}

<COMMENT>"-->" {
		// HTML megjegyzÈs vÈge
		BEGIN(CONTENT);
	}

		/* HTML megjegyzÈsek tartalm·nak tˆrlÈse */
<COMMENT>.|\n


<CONTENT>"<META" {
		// META cÌmke kezdete
		meta = 1;
		printf("%s",yytext);
		BEGIN(ELEMENT);
	}


<CONTENT>"<"[/]* {
		// HTML cÌmke kezdete
		meta = 0;
		printf("%s",yytext);
		BEGIN(ELEMENT);
	}

<ELEMENT>">" {
		// HTML cÌmke vÈge
		printf("%s",yytext);
		BEGIN(CONTENT);
	}
<ELEMENT>[^>a-zA-Z] {
		// HTML cÌmkÈn bel¸l, elemnÈven t˙l
		BEGIN(TAG);
	}

<TAG>">" {
		// HTML cÌmke vÈge
		printf("%s",yytext);
		BEGIN(CONTENT);
	}

		/* Tˆrl¸nk mindent a cÌmkÈbıl az elemnÈven kÌv¸l */
<TAG>[^>]

		/* attrib˙tumokban lehet < Ès > jel, ez is lekezelve */
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
		// decim·lis kÛd alapj·n Windows-125x felismerÈse
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

	    /* nem tˆrı szÛkˆzˆk ·talakÌt·sa */
	    
<CONTENT>"&nbsp"";"? {
		printf(" ");
}

	    /* hosszabb kÛddal nem foglalkozunk */
	    
<CONTENT>"&#"[0-9]*";"? {
		ECHO;
}

	    /* karakter alapj·n Windows-125x felismerÈse */

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

	/* magyar Èkezetes Unicode karakterek felismerÈse */ 

<CONTENT>("√∂"|"√º"|"√≥"|"≈ë"|"√∫"|"√©"|"√°"|"≈±"|"√≠"|"√ñ"|"√ú"|"√ì"|"≈ê"|"√ö"|"√â"|"√Å"|"≈∞"|"√ç") {
	ECHO;
	if (code[1] == 'u') code[3] = 'm';
}
