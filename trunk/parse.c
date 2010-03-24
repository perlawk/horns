#include "parse.h"

node *yylval;
tok_type LOOKAHEAD;
int PARSE_ERROR;

extern node *NODE_RESULT;
int NOTHING_TO_DO;

mode_type MODE;

extern int yylex(void);

void parse() {
	PARSE_ERROR=0;
	NOTHING_TO_DO=0;
	LOOKAHEAD=yylex();

	if (LOOKAHEAD == EOF_TOK) NOTHING_TO_DO=1;
	else program();
}

void program() {
	node *e;

	while (LOOKAHEAD != EOF_TOK && !PARSE_ERROR) {
		e=_exp();
		NODE_RESULT=node_copy(node_do(e));
		node_del(e);
	}
}

node *_exp() {
	node *t;

	switch(LOOKAHEAD) {
		case EOF_TOK:
			error();
			break;
		case NUMBER_TOK:
		case ID_TOK:
		case SYMBOL_TOK:
		case STRING_TOK:
			t=yylval;
			match(LOOKAHEAD);
			return t;
		case LPAREN_TOK:
			return sexp();
		case RPAREN_TOK:
			match(RPAREN_TOK);
			error();
			break;
		case BAD_TOK:
			while (LOOKAHEAD == BAD_TOK) { match(BAD_TOK); }
			error();
	}

	return node_nil();
}

node *sexp() {
	match(LPAREN_TOK);

	node *l=node_empty_list();

	while (LOOKAHEAD != RPAREN_TOK && !PARSE_ERROR) {
		l=node_append(l, _exp());
	}

	match(RPAREN_TOK);

	if (PARSE_ERROR) {
		node_del(l);
		return node_nil();
	}

	return l;
}

void error() { yyerror("syntax error."); PARSE_ERROR=1; NODE_RESULT=node_nil(); }
