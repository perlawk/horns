%{
	#include <stdio.h>
	#include "node.h"

	extern node *TAIL;
	extern node *NODE_RESULT;
	extern int NOTHING_TO_DO;
	extern int yylex(void);
%}

%union {
	double n;
	char *s;
	node *p;
}

%token<n> NUMBER

%token<s> ID STRING QUOTE BAD error

%type<p> exp seq list statement

%start program

%%

program: statement
	| { NOTHING_TO_DO=1; }
	| BAD { NODE_RESULT=node_nil(); yyerror("syntax error"); YYABORT; }
	| error { NOTHING_TO_DO=1; NODE_RESULT=node_nil(); YYABORT; }
	;

statement: exp { NODE_RESULT=node_do($1); $$ = NODE_RESULT; }
	| statement exp { NODE_RESULT=node_do($2); $$ = NODE_RESULT; }
	;

seq: seq exp {
		$$ = args_append($1, $2);
	}
	| { $$ = TAIL; }
	;

list: '(' seq ')' {
		if ($2->type == TAIL_TYPE) $$ = node_empty_list();
		else $$ = $2;
	}
	;

exp: NUMBER { $$ = node_num($1); }
	| ID { $$ = node_id($1); }
	| QUOTE ID { $$ = node_id2sym(node_id($2)); }
	| STRING { $$ = node_str(from_literal($1)); }
	| list { $$ = $1; }
	;

%%