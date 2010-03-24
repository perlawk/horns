#ifndef PARSE_H
#define PARSE_H
#include "tokens.h"
#include "node.h"

extern node *yylval;
extern tok_type LOOKAHEAD;
extern int PARSE_ERROR;

extern int yylex(void);

#define match(T) if (T == LOOKAHEAD) LOOKAHEAD=yylex(); else error();

void parse();
void program();
node *_exp();
node *sexp();

void error();

#endif
