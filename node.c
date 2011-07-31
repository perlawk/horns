#include <stdlib.h>
#include <stdio.h>
#include <gc.h>
#include "node.h"

node *TAIL;
node *SYM_STACK;
node *NODE_RESULT;
bool NOTHING_TO_DO;
extern int yylineno;
extern mode_type MODE;

char *alloc_string(int len) {
	return (char *) GC_MALLOC(sizeof(char)*(len+1));
}

int yyerror(char *s) {
	char *msg;

	if (streq(s, "syntax error")) {
		msg=strdup("syntax error.");
	}
	else {
		msg = strdup(s);
	}

	if (MODE != TEST_MODE) {
		if (MODE == SCRIPT_MODE) fprintf(stderr, "%s (line %d)\n", msg, yylineno);
		else fprintf(stderr, "%s\n", msg);
	}

	NOTHING_TO_DO=true;

	return 0;
}

void node_init() {
	GC_INIT();

	TAIL=(node *) GC_MALLOC(sizeof(node));
	TAIL->type=TAIL_TYPE;

	SYM_STACK=node_empty_list();
	SYM_STACK=node_push(SYM_STACK, node_hash(node_empty_list(), node_empty_list()));

	NODE_RESULT=node_nil();
}

node *node_new() {
	node *temp=(node *) GC_MALLOC(NODE_SIZE);

	if (temp == NULL) {
		(void) yyerror("error: out of memory.");
		exit(0);
	}

	return temp;
}

// void node_del(node *p) {
// 	switch(p->type) {
// 		case TAIL_TYPE:
// 		case BUILTIN_TYPE:
// 			return;
// 		case NIL_TYPE:
// 		case TRUE_TYPE:
// 		case NUMBER_TYPE:
// 			break;
// 		case SYMBOL_TYPE:
// 		case ID_TYPE:
// 		case STRING_TYPE:
// 			// free(p->str);
// 			break;
// 		case LIST_TYPE:
// 		case LAMBDA_TYPE:
// 			node_del(p->first);
// 			node_del(p->rest);
// 			break;
// 	}
// 
// 	// free(p);
// }

node *node_nil() {
	node *p=node_new();
	p->type=NIL_TYPE;
	return p;
}

node *node_true() {
	node *p=node_new();
	p->type=TRUE_TYPE;
	return p;
}

node *node_num(double d) {
	node *p=node_new();
	p->type=NUMBER_TYPE;
	p->n=d;
	return p;
}

node *node_sym(char *s) {
	node *p=node_new();
	p->type=SYMBOL_TYPE;
	p->str=strdup(s);
	return p;
}

node *node_id(char *s) {
	node *p=node_new();
	p->type=ID_TYPE;
	p->str=strdup(s);
	return p;
}

bool is_special_char(char c) {
	switch(c) {
		case '\b': return true;
		case '\t': return true;
		case '\n': return true;
		case '\v': return true;
		case '\f': return true;
		case '\r': return true;
		case '\a': return true;
		case '\'': return true;
		case '\"': return true;
		case '\\': return true;
		default: return false;
	}
}

char unescape_special(char c) {
	/* escape characters from http://www.wilsonmar.com/1eschars.htm */
	switch(c) {
		case 'b': return '\b';
		case 't': return '\t';
		case 'n': return '\n';
		case 'v': return '\v';
		case 'f': return '\f';
		case 'r': return '\r';
		case 'a': return '\a';
		default: return c; /* handles \, ', and " */
	}
}

char escape_special(char c) {
	switch(c) {
		case '\b': return 'b';
		case '\t': return 't';
		case '\n': return 'n';
		case '\v': return 'v';
		case '\f': return 'f';
		case '\r': return 'r';
		case '\a': return 'a';
		default: return c; /* handles \, ', and " */
	}
}

char *from_literal(char *s) {
	int len=(int) strlen(s);

	char *result=alloc_string(len-2);

	bool in_escape=false;

	int i, j;
	for (i=0, j=1; j<len-1; j++) { /* strip outer quotes */
		char c=s[j];

		if (in_escape) {
			result[i++]=unescape_special(c);
			in_escape=0;
		}
		else if (c=='\\') {
			in_escape=true;
		}
		else {
			result[i++]=c;
		}
	}

	return result;
}

char *to_literal(char *s) {
	int len=(int) strlen(s);

	int i, j;

	char *result=alloc_string(len*2+2); /* expect all special characters */

	/* add beginning quote */
	result[0]='\"';

	for (i=1, j=0; j<len; j++) {
		char c=s[j];

		if (is_special_char(c)) {
			result[i++]='\\';
			result[i++]=escape_special(c);
		}
		else {
			result[i++]=c;
		}
	}

	/* add end quote */
	result[i]='\"';

	return result;
}

node *node_str(char *s) {
	node *p=node_new();
	p->type=STRING_TYPE;
	p->str=strdup(s);
	return p;
}

node *node_copy(node *p) {
	node *new;

	switch(p->type) {
		case NIL_TYPE:
			return node_nil();
		case TRUE_TYPE:
			return node_true();
		case NUMBER_TYPE:
			return node_num(p->n);
		case SYMBOL_TYPE:
			return node_sym(p->str);
		case ID_TYPE:
			return node_id(p->str);
		case STRING_TYPE:
			return node_str(p->str);
		case LIST_TYPE:
			new=node_empty_list();
			new->first=node_copy(p->first);
			new->rest=node_copy(p->rest);
			return new;
		case BUILTIN_TYPE:
			return node_builtin(p->str, p->f);
		case TAIL_TYPE:
		default:
			return p;
	}
}

int node_length(node *p) {
	int len;

	node *temp, *rest;

	if (p->type == STRING_TYPE) return (int) strlen(p->str);

	if (node_is_atom(p)) return 0;

	if (p->first->type == TAIL_TYPE) return 0;

	len=1;
	temp=p;
	rest=temp->rest;

	while (rest->type != TAIL_TYPE) {
		temp=rest;
		rest=temp->rest;
		len++;
	}

	return len;
}

int node_cmp(node *a, node *b) {
	int diff;
	double doubleDiff;
	bool aExists, bExists;
	node *aTemp, *bTemp;

	if (a->type == ID_TYPE && node_id_exists(a)) a=node_get(a);
	if (b->type == ID_TYPE && node_id_exists(b)) b=node_get(b);

	if (a->type != b->type) return a->type - b->type;

	switch(a->type) {
		case NIL_TYPE:
		case TRUE_TYPE:
			return 0;
		case NUMBER_TYPE:
			doubleDiff=a->n - b->n;
			if (doubleDiff < 0) return -1;
			else if (doubleDiff > 0) return 1;
			else return 0;
		case SYMBOL_TYPE:
		case STRING_TYPE:
			return strcmp(a->str, b->str);
		case ID_TYPE:
			aExists=node_id_exists(a);
			bExists=node_id_exists(b);

			if (!aExists && !bExists) return strcmp(a->str, b->str);

			if (aExists && !bExists) return 1;
			else if (!aExists && bExists) return -1;
			else return node_cmp(node_get(a), node_get(b));
		case BUILTIN_TYPE:
			return strcmp(a->str, b->str);
		case LIST_TYPE:
		case LAMBDA_TYPE:
			aTemp=a->first;
			bTemp=b->first;
			diff=node_cmp(aTemp, bTemp);
			if (diff!=0) return diff;

			aTemp=a->rest;
			bTemp=b->rest;
			diff=node_cmp(aTemp, bTemp);
			if (diff!=0) return diff;

			return 0;
		case TAIL_TYPE:
			return 0;
		default:
			return 1;
	}
}

node *node_empty_list() {
	node *new=node_new();
	new->type=LIST_TYPE;
	new->first=TAIL;
	new->rest=TAIL;
	return new;
}

node *args_append(node *arg1, node *arg2) {
	if (arg1->type != LIST_TYPE) {
		node *new=node_empty_list();
		new->first=arg2;
		return new;
	}

	return node_append(arg1, arg2);
}

node *node_append(node *p, node *e) {
	node *new, *temp, *rest;

	if (e->type == TAIL_TYPE) return p;

	if (p->type == STRING_TYPE && e->type == STRING_TYPE) {
		char *buffer=alloc_string(strlen(p->str)+strlen(e->str));
		strcat(buffer, p->str);
		strcat(buffer, e->str);
		temp=node_str(buffer);
		// free(buffer);
		return temp;
	}
	else if (p->type == LIST_TYPE) {
		if (p->first->type == TAIL_TYPE) {
			new=node_empty_list();
			new->first=e;
			return new;
		}

		new=node_copy(p);
		temp=new;
		rest=temp->rest;

		while (rest->type != TAIL_TYPE) {
			temp=rest;
			rest=temp->rest;
		}

		temp->rest=node_empty_list();
		temp->rest->first=node_copy(e);

		return new;
	}

	return p;
}

node *node_prepend(node *p, node *e) {
	if (e->type == TAIL_TYPE) return p;

	if (p->type == STRING_TYPE && e->type == STRING_TYPE) {
		return node_append(e, p);
	}
	else if (p->type == LIST_TYPE) {
		if (p->first->type == TAIL_TYPE) {
			node *new=node_copy(p);
			new->first=node_copy(e);
			return new;
		}
		else {
			node *new=node_empty_list();
			new->first=node_copy(e);
			new->rest=node_copy(p);
			return new;
		}
	}

	return p;
}

node *node_cat(node *args) {
	if (args->first->type == STRING_TYPE) {
		node *result=args->first;

		node *temp;

		while (args->rest->type != TAIL_TYPE) {
			args=args->rest;

			temp=args->first;

			if (temp->type != STRING_TYPE) return result;

			result=node_append(result, temp);
		}

		return result;
	}
	else if (args->first->type == LIST_TYPE) {
		node *result=args->first;

		node *temp;

		while (args->rest->type != TAIL_TYPE) {
			args=args->rest;

			temp=args->first;

			if (temp->type != LIST_TYPE) return result;

			result=node_append(result, temp->first);

			while (temp->rest->type != TAIL_TYPE) {
				temp=temp->rest;
				result=node_append(result, temp->first);
			}
		}

		return result;
	}
	else {
		return node_nil();
	}
}

node *node_id2sym(node *s) {
	node *temp;

	char *buffer=alloc_string(strlen(s->str)+1);
	strcat(buffer, "\'");
	strcat(buffer, s->str);
	temp=node_sym(buffer);
	// free(buffer);
	return temp;
}

bool node_id_exists(node *s) {
	node *sym, *level;

	if (s->type != ID_TYPE) return false;

	sym=node_id2sym(s);

	level=SYM_STACK;

	while (level->type != TAIL_TYPE) {
		if (node_is_in(level->first->first, sym)) return true;
		level=level->rest;
	}

	return false;
}

node *node_get(node *s) {
	node *sym, *level;

	char *msg;

	// pass through
	if (s->type != ID_TYPE) return s;

	sym=node_id2sym(s);

	level=SYM_STACK;
	while (level->type != TAIL_TYPE) {
		if (node_is_in(level->first->first, sym)) {
			return node_hash_get(level->first, sym);
		}

		level=level->rest;
	}

	msg=alloc_string(strlen(s->str)+26);
	(void) snprintf(msg, strlen(s->str)+26, "error: id %s is not defined.", s->str);
	yyerror(msg);
	// free(msg);

	return node_nil();
}

node *node_set(node *s, node *v) {
	char *msg_temp, *msg;

	node *level;

	if (s->type != SYMBOL_TYPE) {
		msg_temp=node_string(s, 0);
		msg=alloc_string(strlen(msg_temp)+45);
		sprintf(msg, "error can only map symbols (not %s) to values.", msg_temp);
		// free(msg_temp);
		yyerror(msg);
		// free(msg);

		return node_nil();
	}

	// Is the symbol already set in an outer scope?

	level=SYM_STACK;
	while (level->type != TAIL_TYPE) {
		if (node_is_in(level->first->first, s)) {
			level->first = node_hash_set(level->first, s, v);
			return v;
		}

		level=level->rest;
	}

	SYM_STACK->first=node_hash_set(SYM_STACK->first, s, v);

	return v;
}

node *node_first(node *args) {
	node *temp;

	if (args->type == STRING_TYPE) {
		char *buffer=alloc_string(1);
		buffer[0]=args->str[0];
		temp=node_str(buffer);
		// free(buffer);
		return temp;
	}
	else if (args->type == LIST_TYPE) {
		if (node_length(args)<1) return node_nil();

		return node_copy(args->first);
	}
	else {
		return node_nil();
	}
}

node *node_rest(node *args) {
	if (args->type == STRING_TYPE) {
		return node_str((char *) (args->str+1));
	}
	else if (args->type == LIST_TYPE) {
		if (node_length(args)<1) return node_nil();

		if (args->rest->type == TAIL_TYPE) return node_nil();

		return node_copy(args->rest);
	}
	else {
		return node_nil();
	}
}

node *node_last(node *args) {
	node *temp, *rest;

	if (args->type == STRING_TYPE) {
		char *buffer=alloc_string(1);
		buffer[0]=args->str[strlen(args->str)-1];
		buffer[1]='\0';
		temp=node_str(buffer);
		// free(buffer);
		return temp;
	}
	else if (args->type == LIST_TYPE) {
		if (node_length(args)<1) return node_nil();

		temp=args;
		rest=temp->rest;

		while (rest->type != TAIL_TYPE) {
			temp=rest;
			rest=temp->rest;
		}

		return node_copy(temp->first);
	}
	else {
		return node_nil();
	}
}

int node_index(node *p, node *e) {
	char *t;
	int i;

	if (p->type == STRING_TYPE) {
		if (strlen(e->str) == 0) return -1;

		t=strstr(p->str, e->str);

		if (t == NULL) return -1;

		return t - p->str;
	}
	else if (p->type == LIST_TYPE) {
		i=0;

		while (p->type != TAIL_TYPE) {
			if (node_cmp(p->first, e) == 0) return i;
			p=p->rest;
			i++;
		}
	}

	return -1;
}

int node_count(node *p, node *e) {
	int count=0;

	if (p->type != LIST_TYPE) return 0;

	if (node_length(p) == 0) return 0;

	if (node_cmp(p->first, e) == 0) count++;

	while (p->rest != TAIL) {
		p=p->rest;
		if (node_cmp(p->first, e) == 0) count++;
	}

	return count;
}

node *node_push(node *p, node *e) {
	*p=*(node_prepend(p, e)); return p;
}

node *node_pop(node *p) {
	node *temp=node_first(p);
	*p=*(node_copy(node_rest(p)));
	return temp;
}

node *node_at(node *p, int index) {
	int len, i;

	if (p->type != LIST_TYPE) return node_nil();

	len=node_length(p);

	if (index<0) index=len+index;

	if (index < 0 || index >= len) return node_nil();

	for (i=0; i<index; i++) {
		p=p->rest;
		if (p->type == TAIL_TYPE) return node_nil();
	}

	return p->first;
}

node *node_hash(node *k, node *v) {
	node *new=node_empty_list();
	new=node_append(new, k);
	new=node_append(new, v);
	return new;
}

node *node_hash_get(node *h, node *k) {
	int index;

	if (h->type == ID_TYPE) h=node_get(h);

	if (h->type == NIL_TYPE) return h;

	index=node_index(h->first, k);

	if (index == -1) return node_nil();

	return node_at(h->rest->first, index);
}

node *node_hash_set(node *h, node *k, node *v) {
	int index;

	node *at, *keys, *values;

	if (h->type == ID_TYPE) h=node_get(h);

	if (h->type == NIL_TYPE) return h;

	index=node_index(h->first, k);

	if (index == -1) {
		keys=h->first;
		values=h->rest->first;
		*keys=*(node_append(keys, k));
		*values=*(node_append(values, v));
	}
	else {
		at=node_hash_get(h, k);
		*at=*v;
	}

	return h;
}

char *node_string_join(node *p, char *sep) {
	char *buffer="", *bufferSave, *tempString;
	int sep_len=strlen(sep);
	node *tempNode;

	if (p->type != LIST_TYPE) return node_string(p, 0);

	if (node_length(p) == 0) return "";

	tempNode=p->first;

	tempString=node_string(tempNode, 0);

	buffer=alloc_string(strlen(tempString)+sep_len);

	strcat(buffer, tempString);
	// free(tempString);
	strcat(buffer, sep);

	tempNode=p->rest;

	while (tempNode->type != TAIL_TYPE) {
		bufferSave=strdup(buffer);

		tempString=node_string(tempNode->first, 0);

		buffer=alloc_string(strlen(bufferSave)+strlen(tempString)+sep_len);
		strcat(buffer, bufferSave);
		// free(bufferSave);
		strcat(buffer, tempString);
		// free(tempString);
		strcat(buffer, sep);

		tempNode=tempNode->rest;
	}

	buffer[strlen(buffer)-sep_len]='\0';
	return buffer;
}

char *node_string(node *p, int translateIDs) {
	char *buffer, *bufferSave, *tempString, *tempString2;
	node *tempNode;

	switch(p->type) {
		case NIL_TYPE:
			return strdup("nil");
		case TRUE_TYPE:
			return strdup("true");
		case NUMBER_TYPE:
			buffer=alloc_string(32);
			(void) snprintf(buffer, 32, "%g", p->n);
			return buffer;
		case SYMBOL_TYPE:
			return strdup(p->str);
		case ID_TYPE:
			if (translateIDs && node_id_exists(p)) return node_string(node_get(p), translateIDs);
			else return strdup(p->str);
		case STRING_TYPE:
			return to_literal(p->str);
		case LIST_TYPE:
			if (node_length(p) == 0) return strdup("()");

			tempNode=p->first;

			tempString=node_string(tempNode, translateIDs);

			buffer=alloc_string(strlen(tempString)+2);

			strcat(buffer, "(");
			strcat(buffer, tempString);
			// free(tempString);
			strcat(buffer, " ");

			tempNode=p->rest;

			while (tempNode->type != TAIL_TYPE) {
				bufferSave=strdup(buffer);

				tempString=node_string(tempNode->first, translateIDs);

				buffer=alloc_string(strlen(bufferSave)+strlen(tempString)+1);
				strcat(buffer, bufferSave);
				// free(bufferSave);
				strcat(buffer, tempString);
				// free(tempString);
				strcat(buffer, " ");

				tempNode=tempNode->rest;
			}

			buffer[strlen(buffer)-1]=')';
			return buffer;
		case TAIL_TYPE:
			return "tail";
		case BUILTIN_TYPE:
			return strdup(p->str);
		case LAMBDA_TYPE:
			tempString=node_string(p->first, 0);
			tempString2=node_string_join(p->rest->first, " ");
			buffer=alloc_string(strlen(tempString)+strlen(tempString2)+10);
			strcat(buffer, "(lambda ");
			strcat(buffer, tempString);
			// free(tempString);
			strcat(buffer, " ");
			strcat(buffer, tempString2);
			// free(tempString2);
			strcat(buffer, ")");
			return buffer;
		default:
			return "unknown";
	}
}

node *node_do_lambda(node *l, node *args) {
	node *vars, *exps, *new_scope, *k, *v, *res;

	int pushed_scope, var_len, arg_len;

	char *msg;

	args=node_do_list(args);

	vars=l->first;
	exps=l->rest->first;

	pushed_scope=0;

	var_len=0;
	if (vars->type == LIST_TYPE) var_len=node_length(vars);

	arg_len=0;
	if (args->type == LIST_TYPE) arg_len=node_length(args);

	if (var_len != arg_len) {
		msg=alloc_string(40+30);
		(void) snprintf(msg, 40+30, "error: this lambda requires %d parameters.", var_len);
		yyerror(msg);
		// free(msg);
		return node_nil();
	}

	if (args->type == LIST_TYPE) {
		pushed_scope=1;

		new_scope=node_hash(node_empty_list(), node_empty_list());
		SYM_STACK=node_push(SYM_STACK, new_scope);

		while (vars->type != TAIL_TYPE) {
			k=vars;
			v=args;

			k=node_id2sym(k->first);
			v=v->first;

			node_set(k, v);

			vars=vars->rest; args=args->rest;
		}
	}

	res=node_last(node_do_list(exps));

	if (pushed_scope) node_pop(SYM_STACK);

	return res;
}

node *node_do(node *args) {
	node *op;

	char *op_s, *msg;

	// pass variables through
	if (args->type == ID_TYPE) return node_get(args);

	// pass primitives through
	if (args->type != LIST_TYPE) return args;

	op=args->first;

	// retrieve first argument
	if (op->type == ID_TYPE) {
		if (node_id_exists(op)) {
			op=node_get(op);
		}
		else {
			op_s=strdup(args->first->str);
			msg=alloc_string(strlen(op_s)+17);
			(void) snprintf(msg, strlen(op_s)+17, "unknown function: %s", op_s);
			// free(op_s);
			yyerror(msg);
			// free(msg);

			return node_nil();
		}
	}

	// construct lambda
	if (op->type == LIST_TYPE) {
		if (op->first->type == ID_TYPE) {
			if (streq(op->first->str, "lambda")) {
				op=node_do(op);
			}
			else {
				op_s=strdup(op->first->str);
				msg=alloc_string(strlen(op_s)+17);
				(void) snprintf(msg, strlen(op_s)+17, "unknown function: %s", op_s);
				// free(op_s);
				yyerror(msg);
				// free(msg);

				return node_nil();
			}
		}
		else {
			yyerror("first parameter is not a function");
			return node_nil();
		}
	}

	switch(op->type) {
		case BUILTIN_TYPE:
			return op->f(node_rest(args));
		case LAMBDA_TYPE:
			return node_do_lambda(op, node_rest(args));
		default:
			(void) yyerror("error: lists must be quoted.");
			return node_nil();
	}
}

node *node_do_list(node *args) {
	node *new, *temp;

	if (args->type == NIL_TYPE) return args;

	new=node_empty_list();
	temp=args;
	new=node_append(new, node_do(temp->first));

	temp=temp->rest;
	while (temp->type != TAIL_TYPE) {
		new=node_append(new, node_do(temp->first));
		temp=temp->rest;
	}

	return new;
}

node *node_lambda(node *args, node *exps) {
	node *lambda=node_empty_list();
	lambda=node_append(lambda, args);
	lambda=node_append(lambda, exps);

	lambda->type=LAMBDA_TYPE;
	return lambda;
}

node *node_builtin(char *name, node *(*f)(node *args)) {
	node *new=node_new();
	new->type=BUILTIN_TYPE;
	new->str=strdup(name);
	new->f=f;
	return new;
}

void node_build(char *name, node *(*f)(node *args)) {
	node *s, *b;

	s=node_id2sym(node_id(name));
	b=node_builtin(name, f);

	(void) node_set(s, b);
}
