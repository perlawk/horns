#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "libhorns.h"

extern char **environ;

void horns_init() {
	node_init();

	node_set(node_sym("\'nil"), node_nil());
	node_set(node_sym("\'true"), node_true());
	node_set(node_sym("\'pi"), node_num(M_PI));
	node_set(node_sym("\'e"), node_num(M_E));
	node_set(node_sym("\'VERSION"), node_str(VERSION));

	node_build("!=", node_neq);
	node_build("%", node_mod);
	node_build("&", node_band);
	node_build("*", node_mul);
	node_build("**", node_pow);
	node_build("+", node_add);
	node_build("-", node_sub);
	node_build("/", node_div);
	node_build("<", node_lt);
	node_build("<=", node_lte);
	node_build("=", node_eq);
	node_build(">", node_gt);
	node_build(">=", node_gte);
	node_build("^", node_bxor);
	node_build("abs", node_abs);
	node_build("and", node_and);
	node_build("append", f_append);
	node_build("at", f_at);
	node_build("atom?", f_is_atom);
	node_build("block", node_block);
	node_build("cat", f_cat);
	node_build("ceil", node_ceil);
	node_build("chomp", f_chomp);
	node_build("chop", f_chop);
	node_build("compare", f_compare);
	node_build("copy", f_copy);
	node_build("cos", node_cos);
	node_build("count", f_count);
	node_build("def", node_def);
	node_build("empty?", node_is_empty);
	node_build("env", node_env);
	node_build("exec", node_exec);
	node_build("exists?", f_id_exists);
	node_build("exit", node_exit);
	node_build("first", f_first);
	node_build("floor", node_floor);
	node_build("for", node_for);
	node_build("hash", f_hash);
	node_build("hash?", f_is_hash);
	node_build("hash-get", f_hash_get);
	node_build("hash-keys", node_hash_keys);
	node_build("hash-set", f_hash_set);
	node_build("hash-values", node_hash_values);
	node_build("if", node_if);
	node_build("in?", f_is_in);
	node_build("index", f_index);
	node_build("input", node_input);
	node_build("join", f_join);
	node_build("lambda", f_lambda);
	node_build("last", f_last);
	node_build("let", node_let);
	node_build("length", f_length);
	node_build("list?", f_is_list);
	node_build("ln", node_ln);
	node_build("log", node_log);
	node_build("number", node_str2num);
	node_build("not", node_not);
	node_build("or", node_or);
	node_build("pop", f_pop);
	node_build("prepend", f_prepend);
	node_build("print", node_print);
	node_build("push", f_push);
	node_build("rest", f_rest);
	node_build("set", f_set);
	node_build("sin", node_sin);
	node_build("split", f_split);
	node_build("sqrt", node_sqrt);
	node_build("string", f_string);
	node_build("time", node_time);
	node_build("while", node_while);
	node_build("|", node_bor);
	node_build("~", node_bnot);
}

int param_fail(char *f, node *args, int min, int max) {
	int l=node_length(args);

	if ((min > -1 && l < min) || (max > -1 && l > max)) {
		char *msg=alloc_string(strlen(f)+48);
		sprintf(msg, "error: %s used improperly. check reference manual.", f);
		yyerror(msg);
		free(msg);
		return 1;
	}
	else {
		return 0;
	}
}

int type_fail(char *f, node_type a, node_type b) {
	if (a != b) {
		char *msg=alloc_string(strlen(f)+48);
		sprintf(msg, "error: %s used improperly. check reference manual.", f);
		yyerror(msg);
		free(msg);
		return 1;
	}
	else {
		return 0;
	}
}

int list_type_fail(char *f, node *args, node_type t) {
	node *temp=args;

	if (args->type != LIST_TYPE) return 1;

	if (node_length(args) == 0) return 0;

	while (temp->type != TAIL_TYPE) {
		if (temp->first->type != t) {
			char *msg=alloc_string(strlen(f)+48);
			sprintf(msg, "error: %s used improperly. check reference manual.", f);
			yyerror(msg);
			free(msg);
			return 1;
		}

		temp=temp->rest;
	}

	return 0;
}

node *node_str2num(node *args) {
	if (param_fail("number", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (type_fail("number", args->first->type, STRING_TYPE)) return node_nil();

	return node_num(atof(args->first->str));
}

node *f_copy(node *args) {
	if (param_fail("copy", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	return node_copy(args->first);
}

node *f_is_list(node *args) {
	if (param_fail("list?", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (node_is_list(args->first)) return node_true();
	else return node_nil();
}

node *f_is_atom(node *args) {
	if (param_fail("atom?", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (node_is_atom(args->first)) return node_true();
	else return node_nil();
}

node *f_length(node *args) {
	if (param_fail("length", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	int len=node_length(args->first);

	if (len == -1) return node_nil();
	else return node_num((double) len);
}

node *node_is_empty(node *args) {
	if (param_fail("empty?", args, 1, 1)) return node_nil();

	args=args->first;

	if (node_length(args) == 0) return node_true();
	else return node_nil();
}

node *f_compare(node *args) {
	if (param_fail("compare", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_num((double) node_cmp(args->first, args->rest->first));
}

node *node_bnot(node *args) {
	if (param_fail("~", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (type_fail("~", args->first->type, NUMBER_TYPE)) return node_nil();

	return node_num((double) ~((int) args->first->n));
}

node *node_band(node *args) {
	if (param_fail("&", args, 2, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("&", args, NUMBER_TYPE)) return node_nil();

	int result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result &= (int) args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_bor(node *args) {
	if (param_fail("|", args, 2, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("|", args, NUMBER_TYPE)) return node_nil();

	int result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result |= (int) args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_bxor(node *args) {
	if (param_fail("^", args, 2, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("^", args, NUMBER_TYPE)) return node_nil();

	int result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result ^= (int) args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_neq(node *args) {
	if (param_fail("!=", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (node_cmp(args->first, args->rest->first) != 0) return node_true();
	else return node_nil();
}

node *node_lt(node *args) {
	if (param_fail("<", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (node_cmp(args->first, args->rest->first) < 0) return node_true();
	else return node_nil();
}

node *node_lte(node *args) {
	if (param_fail("<=", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (node_cmp(args->first, args->rest->first) <= 0) return node_true();
	else return node_nil();
}

node *node_eq(node *args) {
	if (param_fail("=", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (node_cmp(args->first, args->rest->first) == 0) return node_true();
	else return node_nil();
}

node *node_gt(node *args) {
	if (param_fail(">", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (node_cmp(args->first, args->rest->first) > 0) return node_true();
	else return node_nil();
}

node *node_gte(node *args) {
	if (param_fail(">=", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (node_cmp(args->first, args->rest->first) >= 0) return node_true();
	else return node_nil();
}

node *f_append(node *args) {
	if (param_fail("append", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_append(args->first, args->rest->first);
}

node *f_prepend(node *args) {
	if (param_fail("prepend", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_prepend(args->first, args->rest->first);
}

node *f_cat(node *args) {
	if (param_fail("cat", args, 2, -1)) return node_nil();

	args=node_do_list(args);

	return node_cat(args);
}

node *f_id_exists(node *args) {
	if (param_fail("exists?", args, 1, 1)) return node_nil();

	if (list_type_fail("exists?", args, ID_TYPE)) return node_nil();

	if (node_id_exists(args->first)) return node_true();
	else return node_nil();
}

node *f_set(node *args) {
	if (param_fail("set", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_set(args->first, args->rest->first);
}

node *node_not(node *args) {
	if (param_fail("not", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (args->first->type == NIL_TYPE) return node_true();
	else return node_nil();
}

node *node_and(node *args) {
	if (param_fail("and", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (args->first->type == NIL_TYPE) return node_nil();

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		if (args->first->type == NIL_TYPE) return node_nil();
		args=args->rest;
	}

	return node_true();
}

node *node_or(node *args) {
	if (param_fail("or", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (args->first->type == TRUE_TYPE) return node_true();

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		if (args->first->type == TRUE_TYPE) return node_true();
		args=args->rest;
	}

	return node_nil();
}

node *node_add(node *args) {
	if (param_fail("+", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("+", args, NUMBER_TYPE)) return node_nil();

	double result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result += args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_sub(node *args) {
	if (param_fail("-", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("-", args, NUMBER_TYPE)) return node_nil();

	double result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result -= args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_mul(node *args) {
	if (param_fail("*", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("*", args, NUMBER_TYPE)) return node_nil();

	double result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result *= args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_div(node *args) {
	if (param_fail("/", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("/", args, NUMBER_TYPE)) return node_nil();

	double result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		if (args->first->n == 0.0) {
			yyerror("error: division by zero.");
			return node_nil();
		}

		result /= args->first->n;
		args=args->rest;
	}

	return node_num(result);
}

node *node_mod(node *args) {
	if (param_fail("%", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("%", args, NUMBER_TYPE)) return node_nil();

	double result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		if (args->first->n == 0.0) {
			yyerror("error: modulus by zero.");
			return node_nil();
		}

		result = fmod(result, args->first->n);
		args=args->rest;
	}

	return node_num(result);
}

node *node_pow(node *args) {
	if (param_fail("**", args, 1, -1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("**", args, NUMBER_TYPE)) return node_nil();

	double result=args->first->n;

	args=args->rest;
	while (args->type != TAIL_TYPE) {
		result = pow(result, args->first->n);
		args=args->rest;
	}

	return node_num(result);
}

node *node_sqrt(node *args) {
	if (param_fail("sqrt", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("sqrt", args, NUMBER_TYPE)) return node_nil();

	if (args->first->n < 0) {
		yyerror("error: sqrt of a negative number.");
		return node_nil();
	}

	return node_num(sqrt(args->first->n));
}

node *node_sin(node *args) {
	if (param_fail("sin", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("sin", args, NUMBER_TYPE)) return node_nil();

	return node_num(sin(args->first->n));
}

node *node_cos(node *args) {
	if (param_fail("cos", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("cos", args, NUMBER_TYPE)) return node_nil();

	return node_num(cos(args->first->n));
}

node *node_ln(node *args) {
	if (param_fail("ln", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("ln", args, NUMBER_TYPE)) return node_nil();

	return node_num(log(args->first->n));
}

node *node_log(node *args) {
	if (param_fail("log", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("log", args, NUMBER_TYPE)) return node_nil();

	return node_num(log10(args->first->n));
}

node *node_floor(node *args) {
	if (param_fail("floor", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("floor", args, NUMBER_TYPE)) return node_nil();

	return node_num(floor(args->first->n));
}

node *node_ceil(node *args) {
	if (param_fail("ceil", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("ceil", args, NUMBER_TYPE)) return node_nil();

	return node_num(ceil(args->first->n));
}

node *node_abs(node *args) {
	if (param_fail("abs", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (list_type_fail("abs", args, NUMBER_TYPE)) return node_nil();

	return node_num(fabs(args->first->n));
}

node *f_first(node *args) {
	if (param_fail("first", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	return node_first(args->first);
}

node *f_rest(node *args) {
	if (param_fail("rest", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	return node_rest(args->first);
}

node *f_last(node *args) {
	if (param_fail("last", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	return node_last(args->first);
}

node *f_index(node *args) {
	if (param_fail("index", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_num((double) node_index(args->first, args->rest->first));
}

node *f_string(node *args) {
	if (param_fail("string", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (args->first->type == STRING_TYPE) return args->first;

	return node_str(node_string(args->first, 1));
}

node *node_chop(node *p) {
	if (p->type != STRING_TYPE) return node_nil();

	node *new=node_copy(p);
	new->str[strlen(new->str)-1]='\0';
	return new;
}

node *node_chomp(node *p) {
	if (p->type != STRING_TYPE) return node_nil();

	int len=strlen(p->str);
	char last=p->str[len-1];

	while (last == '\n' || last == '\r') {
		p=node_chop(p);
		len--;
		last=p->str[len-1];
	}

	return p;
}

node *f_chop(node *args) {
	if (param_fail("chop", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (type_fail("chop", args->first->type, STRING_TYPE)) return node_nil();

	return node_chop(args->first);
}

node *f_chomp(node *args) {
	if (param_fail("chomp", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (type_fail("chomp", args->first->type, STRING_TYPE)) return node_nil();

	return node_chomp(args->first);
}

node *node_input(node *args) {
	if (param_fail("input", args, 0, 1)) return node_nil();

	char *prompt="";

	int len=node_length(args);

	if (len == 1) {
		if (type_fail("input", args->first->type, STRING_TYPE)) return node_nil();

		prompt=args->first->str;
	}

	fprintf(stdout, "%s", prompt);

	char *temp=alloc_string(MAX_LINE);
	temp=fgets(temp, MAX_LINE, stdin);

	return node_chomp(node_str(temp));
}

node *node_exit(node *args) {
	if (param_fail("exit", args, 0, 1)) return node_nil();

	int status=0;

	int len=node_length(args);

	if (len == 1) {
		if (type_fail("input", args->first->type, NUMBER_TYPE)) return node_nil();

		node *f=args->first;
		f=node_get(f);
		status=(int) f->n;
	}

	exit(status);

	return node_nil();
}

node *node_time(node *args) {
	if (param_fail("time", args, 0, 0)) return node_nil();

	args=node_do_list(args);

	time_t timer=time(NULL);
	return node_num((double) timer);
}

node *f_split(node *args) {
	if (param_fail("split", args, 2, 2)) return node_nil();

	args=node_do(args);

	return node_split(args->first->str, args->rest->first->str);
}

node *f_join(node *args) {
	if (param_fail("join", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_join(args->first, args->rest->first);
}

node *f_count(node *args) {
	if (param_fail("count", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (type_fail("count", args->first->type, LIST_TYPE)) return node_nil();

	return node_num((double) node_count(args->first, args->rest->first));
}

node *f_is_in(node *args) {
	if (param_fail("in?", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (type_fail("in?", args->first->type, LIST_TYPE)) return node_nil();

	if (node_is_in(args->first, args->rest->first)) return node_true();
	else return node_nil();
}

node *f_push(node *args) {
	if (param_fail("push", args, 2, 2)) return node_nil();

	node *a=node_do(args->first), *b=node_do(args->rest->first);

	return node_push(a, b);
}

node *f_pop(node *args) {
	if (param_fail("pop", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	return node_pop(args->first);
}

node *f_at(node *args) {
	if (param_fail("at", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	if (type_fail("at", args->first->type, LIST_TYPE)) return node_nil();
	if (type_fail("at", args->rest->first->type, NUMBER_TYPE)) return node_nil();

	return node_at(args->first, (int) args->rest->first->n);
}

node *f_hash(node *args) {
	if (param_fail("hash", args, 2, 2)) return node_nil();

	node_do_list(args);

	return node_hash(args->first, args->rest->first);
}

node *f_is_hash(node *args) {
	if (param_fail("hash?", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (node_is_hash(args->first)) return node_true();
	else return node_nil();
}

node *f_hash_get(node *args) {
	if (param_fail("hash-get", args, 2, 2)) return node_nil();

	args=node_do_list(args);

	return node_hash_get(args->first, args->rest->first);
}

node *f_hash_set(node *args) {
	if (param_fail("hash-set", args, 3, 3)) return node_nil();

	node *a=args->first, *b=node_do(args->rest->first), *c=node_do(args->rest->rest->first);

	return node_hash_set(a, b, c);
}

node *node_hash_keys(node *args) {
	if (param_fail("hash-keys", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (!node_is_hash(args->first)) return node_nil();

	return args->first->first;
}

node *node_hash_values(node *args) {
	if (param_fail("hash-values", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (!node_is_hash(args->first)) return node_nil();

	return args->first->rest->first;
}

node *node_print(node *args) {
	if (param_fail("print", args, -1, -1)) return node_nil();

	args=node_do_list(args);

	NOTHING_TO_DO=1;

	if (args->type == NIL_TYPE) {
		node *res=node_str("\n");
		printf("%s", res->str);
		return res;
	}

	node *tempNode=args->first;

	char *tempString;

	if (tempNode->type != STRING_TYPE) {
		tempString=node_string(tempNode, 1);
	}
	else {
		tempString=tempNode->str;
	}

	char *buffer=alloc_string(strlen(tempString));

	strcat(buffer, tempString);
	free(tempString);

	char *bufferSave;

	tempNode=args->rest;

	while (tempNode->type != TAIL_TYPE) {
		bufferSave=strdup(buffer);

		if (tempNode->first->type != STRING_TYPE) {
			tempString=node_string(tempNode->first, 1);
		}
		else {
			tempString=strdup(tempNode->first->str);
		}

		buffer=alloc_string(strlen(bufferSave)+strlen(tempString));
		strcat(buffer, bufferSave);
		free(bufferSave);
		strcat(buffer, tempString);
		free(tempString);

		tempNode=tempNode->rest;
	}

	printf("%s\n", buffer);

	return node_str(buffer);
}

node *node_split(char *str, char *sep) {
	node *result=node_empty_list();

	if (strlen(sep) == 0) return node_append(result, node_str(str));

	char *str_c=alloc_string(strlen(str));
	strcat(str_c, str);

	char *t, *s;
	int index;

	while (strlen(str_c) > 0) {
		t=strstr(str_c, sep);
		if (t == NULL) {
			result=node_append(result, node_str(str_c));
			break;
		}

	 	index=t-str_c;

		s=alloc_string(index);
		s=strncpy(s, str_c, index);

		str_c+=index+1;

		result=node_append(result, node_str(s));
	}

	return result;
}

node *node_join(node *p, node *e) {
	char *buffer="", *bufferSave, *tempString, *sep;
	int sep_len;
	node *tempNode;

	if (list_type_fail("join", p, STRING_TYPE)) return node_nil();
	if (type_fail("join", e->type, STRING_TYPE)) return node_nil();

	if (node_length(p) == 0) return node_str(buffer);

	sep=e->str;
	sep_len=strlen(sep);

	tempNode=p->first;

	tempString=strdup(tempNode->str);

	buffer=alloc_string(strlen(tempString)+sep_len);

	strcat(buffer, tempString);
	free(tempString);

	tempNode=p->rest;
	if (tempNode->type != TAIL_TYPE) strcat(buffer, sep);

	while (tempNode->type != TAIL_TYPE) {
		bufferSave=strdup(buffer);

		tempString=strdup(tempNode->first->str);

		buffer=alloc_string(strlen(bufferSave)+strlen(tempString)+sep_len);
		strcat(buffer, bufferSave);
		free(bufferSave);
		strcat(buffer, tempString);
		free(tempString);

		tempNode=tempNode->rest;
		if (tempNode->type != TAIL_TYPE) strcat(buffer, sep);
	}

	node *res=node_str(buffer);
	free(buffer);

	return res;
}

node *node_env(node *args) {
	if (param_fail("env", args, 0, 2)) return node_nil();

	args=node_do_list(args);

	int len=node_length(args);

	node *k, *v;

	if (len < 1) {
		node *result=node_hash(node_empty_list(), node_empty_list());
		node *temp;

		int i=0;
		char *var=environ[i];
		while (var != NULL) {
			temp=node_split(var, "=");

			// value was empty
			if (node_length(temp) < 2) temp=node_append(temp, node_str(""));

			// value was split because it contained "="
			if (node_length(temp) > 2) {
				node *temp2=node_empty_list();
				temp2=node_append(temp2, temp->first);
				temp2=node_append(temp2, node_join(temp->rest, node_str("=")));
				temp=temp2;
			}

			result=node_hash_set(result, temp->first, temp->rest->first);
			var=environ[++i];
		}

		return result;
	}
	else {
		k=args->first;

		if (type_fail("env", k->type, STRING_TYPE)) return node_nil();

		if (len == 1) {
			return node_str(getenv(k->str));
		}
		else {
			v=args->rest->first;

			if (type_fail("env", v->type, STRING_TYPE)) return node_nil();

			node *temp=node_empty_list();
			temp=node_append(temp, k);
			temp=node_append(temp, v);
			temp=node_join(temp, node_str("="));

			int x=putenv(temp->str);
			x=x;

			return v;
		}
	}
}

node *node_exec(node *args) {
	if (param_fail("exec", args, 1, 1)) return node_nil();

	args=node_do_list(args);

	if (type_fail("exec", args->first->type, STRING_TYPE)) return node_nil();

	return node_num(system(args->first->str));
}

node *node_if(node *args) {
	if (param_fail("if", args, 2, 3)) return node_nil();

	int len=node_length(args);

	node *cond=args->first;

	node *if_exp=args->rest->first;

	node *else_exp;
	if (len == 2) else_exp=node_nil();
	else else_exp=args->rest->rest->first;

	node *res;

	if (node_do(cond)->type == NIL_TYPE) res=node_do(else_exp);
	else res=node_do(if_exp);

	return res;
}

node *node_for(node *args) {
	if (param_fail("for", args, 2, -1)) return node_nil();

	node *vars=args->first;
	node *exps=args->rest;

	if (param_fail("for", vars, 3, 3)) return node_nil();

	node *fst=vars->first;

	if (type_fail("for", fst->type, ID_TYPE)) return node_nil();

	node *snd=vars->rest->first, *thd=vars->rest->rest->first;

	node *new_scope=node_hash(node_empty_list(), node_empty_list());
	SYM_STACK=node_push(SYM_STACK, new_scope);

	node *i_node=node_set(node_id2sym(fst), node_do(snd));

	int i=(int) i_node->n;

	node *end_i=node_do(thd);

	int end=(int) end_i->n;

	node *res=node_nil();

	while (i < end) {
		res=node_last(node_do_list(exps));

		i_node->n+=1;
		i++;
	}

	node_pop(SYM_STACK);

	return res;
}

node *node_while(node *args) {
	if (param_fail("while", args, 2, -1)) return node_nil();

	node *cond=args->first;
	node *exps=args->rest;

	node *res=node_nil();

	while (node_do(cond)->type != NIL_TYPE) {
		res=node_last(node_do_list(exps));
	}

	return res;
}

node *node_let(node *args) {
	if (param_fail("let", args, 2, -1)) return node_nil();

	node *vars=args->first;
	node *exps=args->rest;

	if (type_fail("let", vars->type, LIST_TYPE)) return node_nil();

	if (node_length(vars)%2 != 0) {
		yyerror("error: let ids must be specified in pairs");
		return node_nil();
	}

	node *new_scope=node_hash(node_empty_list(), node_empty_list());
	SYM_STACK=node_push(SYM_STACK, new_scope);

	while (vars->type != TAIL_TYPE) {
		node *k=vars->first, *v=vars->rest->first;
		node_set(k, v);
		vars=vars->rest->rest;
	}

	node *res=node_last(node_do_list(exps));

	node_pop(SYM_STACK);

	return res;
}

node *f_lambda(node *args) {
	if (param_fail("lambda", args, 2, -1)) return node_nil();

	node *vars=args->first;
	node *exps=args->rest;

	node *temp=vars;

	if (type_fail("lambda", temp->type, LIST_TYPE)) return node_nil();

	if (list_type_fail("lambda", vars, ID_TYPE)) return node_nil();

	return node_lambda(vars, exps);
}

node *node_def(node *args) {
	if (param_fail("def", args, 2, -1)) return node_nil();

	node *vars=args->first;

	if (list_type_fail("def", vars, ID_TYPE)) return node_nil();

	node *exps=args->rest;

	node *name=node_first(vars);
	args=node_rest(vars);

	return node_set(node_id2sym(name), node_lambda(args, exps));
}

node *node_block(node *args) { return node_last(node_do_list(args)); }
