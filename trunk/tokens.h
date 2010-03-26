#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
	EOF_TOK,
	NUMBER_TOK,
	ID_TOK,
	SYMBOL_TOK,
	STRING_TOK,
	LPAREN_TOK,
	RPAREN_TOK,
	BAD_TOK
} tok_type;

#endif
