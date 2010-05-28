#ifndef LIBHORNS
#define LIBHORNS

#include "node.h"

void horns_init();

int param_fail(char *f, node *args, int min, int max);
int type_fail(char *f, node_type a, node_type b);
int list_type_fail(char *f, node *args, node_type t);

node *node_str2num(node *args);
node *f_copy(node *args);
node *f_is_list(node *args);
node *f_is_atom(node *args);
node *f_length(node *args);
node *node_is_empty(node *args);
node *f_compare(node *args);
node *node_bnot(node *args);
node *node_band(node *args);
node *node_bor(node *args);
node *node_bxor(node *args);
node *node_neq(node *args);
node *node_lt(node *args);
node *node_lte(node *args);
node *node_eq(node *args);
node *node_gt(node *args);
node *node_gte(node *args);
node *f_append(node *args);
node *f_prepend(node *args);
node *f_cat(node *args);
node *f_id_exists(node *args);
node *f_set(node *args);
node *node_not(node *args);
node *node_and(node *args);
node *node_or(node *args);
node *node_add(node *args);
node *node_sub(node *args);
node *node_mul(node *args);
node *node_div(node *args);
node *node_mod(node *args);
node *node_pow(node *args);
node *node_sqrt(node *args);
node *node_sin(node *args);
node *node_cos(node *args);
node *node_ln(node *args);
node *node_log(node *args);
node *node_floor(node *args);
node *node_ceil(node *args);
node *node_abs(node *args);
node *f_first(node *args);
node *f_rest(node *args);
node *f_last(node *args);
node *f_index(node *args);
node *f_string(node *args);
node *node_chop(node *p);
node *node_chomp(node *p);
node *f_chop(node *args);
node *f_chomp(node *args);
node *node_input(node *args);
node *node_exit(node *args);
node *node_time(node *args);
node *node_split(char *str, char *sep);
node *node_join(node *p, node *e);
node *f_split(node *args);
node *f_join(node *args);
node *node_env(node *args);
node *node_exec(node *args);
node *node_if(node *args);
node *node_unless(node *args);
node *node_for(node *args);
node *node_while(node *args);
node *node_let(node *args);
node *f_lambda(node *args);
node *node_def(node *args);
node *node_block(node *args);
node *f_count(node *args);
node *f_is_in(node *args);
node *f_push(node *args);
node *f_pop(node *args);
node *f_at(node *args);
node *f_hash(node *args);
node *f_is_hash(node *args);
node *f_hash_get(node *args);
node *f_hash_set(node *args);
node *node_hash_keys(node *args);
node *node_hash_values(node *args);
node *node_print(node *args);
#endif
