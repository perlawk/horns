/*

FreeBSD License

Copyright 2010 Andrew Pennebaker www.yellosoft.us. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef NODE_H
#define NODE_H

#include <string.h>
#define streq(a, b) strcmp(a, b) == 0
#define alloc_string(len) (char *) calloc(len+1, sizeof(char*))

#define VERSION "0.0.4"
#define COPYRIGHT "(c) 2010 Andrew Pennebaker www.yellosoft.us"

#ifdef __x86_64__
	#define ARCH "64bit"
#else
	#define ARCH "32bit"
#endif

#define HOW_TO_EXIT "Press Control-C to exit."
#define PROMPT "> "
#define MAX_LINE 2048

typedef enum {
	TAIL_TYPE,
	NIL_TYPE,
	TRUE_TYPE,
	NUMBER_TYPE,
	SYMBOL_TYPE,
	ID_TYPE,
	STRING_TYPE,
	LIST_TYPE,
	BUILTIN_TYPE,
	LAMBDA_TYPE
} node_type;

typedef struct node_struct {
	node_type type;
	double n;
	char *str;
	struct node_struct *first, *rest;
	struct node_struct *(*f)(struct node_struct *args);
} node;

#define NODE_SIZE sizeof(node)

typedef enum { HELP_MODE, VERSION_MODE, TEST_MODE, INTERACTIVE_MODE, SCRIPT_MODE, EXP_MODE } mode_type;

extern node *TAIL;
extern node *SYM_STACK;
extern node *NODE_RESULT;
extern int NOTHING_TO_DO;

int yyerror(char *s);
void node_init();
node *node_new();
void node_del(node *p);
node *node_nil();
node *node_true();
node *node_num(double d);
node *node_sym(char *s);
node *node_id(char *s);
int is_special_char(char c);
char unescape_special(char c);
char escape_special(char c);
char *from_literal(char *s);
char *to_literal(char *s);
node *node_str(char *s);
node *node_copy(node *p);
#define node_is_list(P) P->type == LIST_TYPE
#define node_is_atom(P) P->type != LIST_TYPE
int node_length(node *p);
int node_cmp(node *a, node *b);
node *node_empty_list();
node *args_append(node *arg1, node *arg2);
node *node_append(node *p, node *e);
node *node_prepend(node *p, node *e);
node *node_cat(node *args);
node *node_id2sym(node *s);
#define node_sym2id(S) node_id((char *) (S->str+1))
int node_id_exists(node *s);
node *node_get(node *s);
node *node_set(node *s, node *v);
node *node_first(node *args);
node *node_rest(node *args);
node *node_last(node *args);
int node_index(node *p, node *e);
int node_count(node *p, node *e);
#define node_is_in(P, E) node_index(P, E) != -1
node *node_push(node *p, node *e);
node *node_pop(node *p);
node *node_at(node *p, int index);
node *node_hash(node *k, node *v);
#define node_is_hash(H) node_is_list(H) && node_length(H) == 2 && H->first->type == LIST_TYPE && H->rest->first->type == LIST_TYPE
node *node_hash_get(node *h, node *k);
node *node_hash_set(node *h, node *k, node *v);
char *node_string_join(node *p, char *sep);
char *node_string(node *p, int translateIDs);
node *node_do(node *args);
node *node_do_list(node *args);
node *node_do_lambda(node *l, node *args);
node *node_lambda(node *args, node *exps);
node *node_builtin(char *name, node *(*f)(node *args));
void node_build(char *name, node *(*f)(node *args));
#endif
