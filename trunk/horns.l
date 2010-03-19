/* Based on "A Compact Guide to Lex & Yacc" (http://epaperpress.com/lexandyacc/) */

%{
	#include "node.h"
	#include "y.tab.h"

	#define YY_NO_INPUT
	#define YYSTYPE yylval;

	extern int NOTHING_TO_DO;
%}

%option yylineno
%option nounput

D [0-9]
H [a-fA-F0-9]
E [Ee][+-]?{D}+

hex 0[xX]{H}+

octal 0{7}+

decimal {D}+

scientific {D}+{E}

double ({D}*"."{D}+({E})?)|({D}+"."{D}*({E})?)

number {hex}|[+\-]?({octal}|{decimal}|{scientific}|{double})

id_char [~&\|\^+\-*/%?<>=!a-zA-Z_]

id {id_char}+({id_char}|{D})*

string \"(\\.|[^\\"])*\"

quote "\'"

comment ;.*|#!.*

space [\n\r\t ]+

misc [()]

bad .

%%

{number} { yylval.n=atof(yytext); return NUMBER; }

{id} { yylval.s=yytext; return ID; }

{string} { yylval.s=yytext; return STRING; }

{quote} { return QUOTE; }

{comment}|{space} ; /* ignore */

{misc} { return *yytext; }

{bad} { NOTHING_TO_DO=1; yylval.s=yytext; return BAD; }

%%

int yywrap(void) {
	return 1;
}
