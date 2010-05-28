/* Based on "A Compact Guide to Lex & Yacc" (http://epaperpress.com/lexandyacc/) */

%{
	#include "tokens.h"
	#include "node.h"

	#define YY_NO_INPUT
	#define YYSTYPE yylval;

	extern node *yylval;
%}

%option yylineno
%option nounput

D [0-9]
H [a-fA-F0-9]
E [Ee][+-]?{D}+

number 0[xX]{H}+|[+\-]?({D}*"."{D}+({E})?)|({D}+("."{D}*)?({E})?)

id_char [~&\|\^+\-*/%?<>=!a-zA-Z_]

id {id_char}+{D}*

symbol "\'"{id}

quote "\'"

string \"(\\.|[^\\"])*\"

lparen "("

rparen ")"

comment ;.*|#!.*

space [\n\r\t ]+

bad .

%%

{number} { yylval=node_num(atof(yytext)); return NUMBER_TOK; }

{id} { yylval=node_id(yytext); return ID_TOK; }

{symbol} { yylval=node_sym(yytext); return SYMBOL_TOK; }

{quote} { yylval=node_id("quote"); return QUOTE_TOK; }

{string} { yylval=node_str(from_literal(yytext)); return STRING_TOK; }

{lparen} { return LPAREN_TOK; }

{rparen} { return RPAREN_TOK; }

{comment}|{space} ; /* ignore */

{bad} { yylval=node_str(yytext); return BAD_TOK; }

%%

int yywrap(void) { return 1; }
