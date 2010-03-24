#include <stdio.h>
#include "test.h"
#include "libhorns.h"
#include "parse.h"

extern node *NODE_RESULT;
extern int SUCCESSES;
extern int FAILURES;

extern int yy_scan_string(char *);

void test_code(char *code, node *result) {
	yy_scan_string(code);
	parse();

	if (node_cmp(NODE_RESULT, result) == 0) SUCCESSES++;
	else {
		FAILURES++;
		printf("FAIL %s => %s\n", code, node_string(NODE_RESULT, 1));
	}
}

void test() {
	test_code("VERSION", node_str(VERSION));
	test_code("()", node_empty_list());
	test_code("(+ 1 2)", node_num(3));
	test_code("(- 9 5)", node_num(4));
	test_code("(* 1 5)", node_num(5));
	test_code("(/ 12 2)", node_num(6));
	test_code("(% 7 8)", node_num(7));
	test_code("(+ 1.5 1.5)", node_num(3));
	test_code("1e+300", node_num(1e+300));
	test_code("(string 1e+300)", node_str("1e+300"));
	test_code("(** 2 3 2)", node_num(64));
	test_code("(sqrt 4)", node_num(2));
	test_code("(sin (/ pi 2))", node_num(1));
	test_code("(cos pi)", node_num(-1));
	test_code("(ln e)", node_num(1));
	test_code("(log 100)", node_num(2));
	test_code("(not true)", node_nil());
	test_code("(not nil)", node_true());
	test_code("(and true true)", node_true());
	test_code("(and true nil)", node_nil());
	test_code("(and true true true nil)", node_nil());
	test_code("(and nil true)", node_nil());
	test_code("(and nil nil)", node_nil());
	test_code("(or true nil)", node_true());
	test_code("(or true true)", node_true());
	test_code("(or nil true)", node_true());
	test_code("(or nil nil)", node_nil());
	test_code("(or nil nil nil true)", node_true());

	node *l=node_empty_list();
	l=node_append(l, node_num(1));
	l=node_append(l, node_num(2));
	l=node_append(l, node_num(3));
	test_code("(push (2 3) 1)", l);
	test_code("(push \"at\" \"c\")", node_str("cat"));
	test_code("(set \'a (2 3)) (push a 1) a", l);
	test_code("(set \'b (pop a)) b", node_num(1));

	l=node_empty_list();
	l=node_append(l, node_num(2));
	l=node_append(l, node_num(3));
	test_code("a", l);

	test_code("(exists? x)", node_nil());
	test_code("(set \'x 5) (exists? x)", node_true());

	test_code("(string nil)", node_str("nil"));
	test_code("(string 5)", node_str("5"));
	test_code("(string 3.14)", node_str("3.14"));
	test_code("(string (1 2 3))", node_str("(1 2 3)"));
	test_code("(string \"hi\")", node_str("hi"));

	test_code("(chomp \"Herb\\n\")", node_str("Herb"));
	test_code("(chomp \"Herb\\r\")", node_str("Herb"));
	test_code("(chomp \"Herb\\r\\n\")", node_str("Herb"));
	test_code("(chomp \"Herb\")", node_str("Herb"));
	test_code("(chop \"Herb\\n\")", node_str("Herb"));
	test_code("(chop \"Herb\\r\")", node_str("Herb"));
	test_code("(chop \"Herb\")", node_str("Her"));

	test_code("(atom? 1)", node_true());
	test_code("(length 1)", node_num(0));
	test_code("(compare 1 2)", node_num(-1));
	test_code("(compare nil nil)", node_num(0));
	test_code("(compare (1 2) (1 2))", node_num(0));
	test_code("(< -1 0)", node_true());
	test_code("(< 1 2)", node_true());
	test_code("(> 2 1)", node_true());
	test_code("(< 2 1)", node_nil());
	test_code("(<= 1 1)", node_true());
	test_code("(>= 2 2)", node_true());
	test_code("(= 1 1)", node_true());
	test_code("(= 1 2)", node_nil());
	test_code("(= (copy (1 2 3)) (1 2 3))", node_true());
	test_code("(= nil true)", node_nil());
	test_code("(!= nil true)", node_true());

	test_code("(= (copy +) +)", node_true());

	l=node_empty_list();
	l=node_append(l, node_num(1));
	l=node_append(l, node_num(2));
	l=node_append(l, node_num(3));
	test_code("(append (1 2) 3)", l);
	test_code("(prepend (2 3) 1)", l);

	test_code("(cat (1) (2) (3))", l);
	test_code("(cat () (1 2) (3))", l);
	test_code("(cat (1 2) (3) ())", l);

	test_code("(cat \"c\" \"a\" \"t\")", node_str("cat"));
	test_code("(cat \"\" \"ca\" \"t\")", node_str("cat"));
	test_code("(cat \"ca\" \"t\" \"\")", node_str("cat"));

	test_code("(length \"hi\")", node_num(2));

	l=node_empty_list();
	l=node_append(l, node_num(1));
	l=node_append(l, node_num(2));
	test_code("(1 2)", l);

	test_code("(atom? (1 2))", node_nil());
	test_code("(length (1 2))", node_num(2));
	test_code("(empty? nil)", node_true());
	test_code("(empty? ())", node_true());
	test_code("(empty? (1))", node_nil());

	test_code("(first (1 2 3))", node_num(1));
	test_code("(first \"abc\")", node_str("a"));

	test_code("(number \"-1\")", node_num(-1));
	test_code("(set \'a 1) (+ a 1)", node_num(2));
	test_code("z", node_nil());
	test_code("(needs-closing-paren", node_nil());

	test_code("(- (* 2 2 2 2 2 2 2 2 2 2) 1022)", node_num(2));

	test_code("(> pi 3)", node_true());
	test_code("(= pi 3)", node_nil());
	test_code("(< e 3)", node_true());
	test_code("(= e 3)", node_nil());

	test_code("(> (time) 0)", node_true());

	l=node_empty_list();
	l=node_append(l, node_nil());
	l=node_append(l, node_true());
	test_code("(nil true)", l);

	test_code("(set \'a (1 2 3)) (first a)", node_num(1));

	l=node_empty_list();
	l=node_append(l, node_num(2));
	l=node_append(l, node_num(3));
	test_code("(set \'a (1 2 3)) (rest a)", l);
	test_code("(set \'a (1 2 3)) (last a)", node_num(3));
	test_code("(list? a)", node_true());
	test_code("(atom? a)", node_nil());

	test_code("(set \'3 2)", node_nil());
	test_code("(* 3 3)", node_num(9));

	test_code("(index \"robert\" \"bert\")", node_num(2));
	test_code("(index (1 2 3) 1)", node_num(0));
	test_code("(index (1 2 3) 4)", node_num(-1));
	test_code("(index \"a b c\" \"\")", node_num(-1));
	test_code("(in? (1 2 3) 1)", node_true());
	test_code("(in? (1 2 3) 4)", node_nil());
	test_code("(at (1 2 3) 0)", node_num(1));
	test_code("(at (1 2 3) 1)", node_num(2));
	test_code("(at (1 2 3) 2)", node_num(3));
	test_code("(at (1 2 3) 3)", node_nil());
	test_code("(count (1 2 3) 1)", node_num(1));
	test_code("(count (1 1 1) 1)", node_num(3));
	test_code("(count (1 2 3) 4)", node_num(0));
	test_code("(count () 1)", node_num(0));

	l=node_empty_list();
	l=node_append(l, node_str("a"));
	l=node_append(l, node_str("b"));
	l=node_append(l, node_str("c"));
	test_code("(split \"a b c\" \" \")", l);

	l=node_empty_list();
	l=node_append(l, node_str("a b c"));
	test_code("(split \"a b c\" \"!\")", l);
	test_code("(split \"a b c\" \"\")", l);
	test_code("(join (\"a\" \"b\" \"c\") \" \")", node_str("a b c"));
	test_code("(join (\"a\" \"b\" \"c\") \", \")", node_str("a, b, c"));
	test_code("(join (\"a\" \"b\" \"c\") \"\")", node_str("abc"));
	test_code("(join (\"a\") \" \")", node_str("a"));
	test_code("(join () \" \")", node_str(""));

	node *k=node_empty_list();
	k=node_append(k, node_str("a"));
	k=node_append(k, node_str("b"));
	k=node_append(k, node_str("c"));

	node *v=node_empty_list();
	v=node_append(v, node_num(1));
	v=node_append(v, node_num(2));
	v=node_append(v, node_num(3));

	node *h=node_hash(k, v);
	test_code("(set \'a (hash (\"a\" \"b\" \"c\") (1 2 3)))", h);

	test_code("(hash? a)", node_true());
	test_code("(hash? (1 2))", node_nil());

	test_code("(hash-keys a)", k);
	test_code("(hash-values a)", v);

	test_code("(set \'a (hash () ())) (hash-set a \"guy\" \"robert\") (hash-get a \"guy\")", node_str("robert"));

	test_code("(hash? (env))", node_true());

	test_code("(env \"COOLNAME\" \"Bobby\")", node_str("Bobby"));
	test_code("(env \"COOLNAME\")", node_str("Bobby"));
	test_code("(env \"COOLNAME\" \"red=blue\")", node_str("red=blue"));
	test_code("(env \"COOLNAME\")", node_str("red=blue"));
	test_code("(hash? (env))", node_true());

	test_code("(if (< 1 2) 1 2)", node_num(1));
	test_code("(if (< 1 2) 1)", node_num(1));
	test_code("(if (> 1 2) 1 2)", node_num(2));
	test_code("(if (> 1 2) 1)", node_nil());

	test_code("(set \'i 0)", node_num(0));

	test_code("(while (< i 5) (set \'i (+ i 1)))", node_num(5));
	test_code("(for (i 0 5) i)", node_num(4));

	test_code("(block (+ 2 2) (- 4 1) (* 1 2) (/ 5 5))", node_num(1));
	test_code("(let (\'y 3) (+ y 2))", node_num(5));
	test_code("y", node_nil());
	test_code("(set \'f (lambda (x) (+ x 1))) (f 4)", node_num(5));

	node *vars=node_empty_list();
	vars=node_append(vars, node_id("x"));
	vars=node_append(vars, node_id("y"));

	node *exps=node_empty_list();
	exps=node_append(exps, node_id("*"));
	exps=node_append(exps, node_id("x"));
	exps=node_append(exps, node_id("y"));

	node *temp=node_empty_list();
	temp=node_append(temp, exps);

	node *lambda=node_lambda(vars, temp);
	test_code("(lambda (x y) (* x y))", lambda);
	test_code("((lambda (x y) (* x y)) 2 3)", node_num(6));
	test_code("(set \'multiply (lambda (x y) (* x y))) (multiply 5)", node_nil());
	test_code("(atom? multiply)", node_true());

	test_code("(set \'a (lambda () 5)) (a)", node_num(5));
	test_code("(set \'a (lambda (x y) (+ x y) (- x y) (* x y) (/ x y))) (a 1 2)", node_num(0.5));

	vars=node_empty_list();
	vars=node_append(vars, node_id("x"));

	exps=node_empty_list();
	exps=node_append(exps, node_id("*"));
	exps=node_append(exps, node_id("x"));
	exps=node_append(exps, node_id("x"));

	temp=node_empty_list();
	temp=node_append(temp, exps);

	node *def=node_lambda(vars, temp);

	test_code("(def (square x) (* x x))", def);
	test_code("(square)", node_nil());
	test_code("(square 4)", node_num(16));

	test_code("(set \'plus +)", node_builtin("+", node_add));
	test_code("(set \'+ -)", node_builtin("-", node_sub));
	test_code("(+ 1 1)", node_num(0));
	test_code("(set '+ plus)", node_builtin("+", node_add));
	test_code("(+ 1 1)", node_num(2));

	if (FAILURES<1) printf("ALL %d TESTS PASS\n", SUCCESSES);
	else printf("%d TESTS FAIL\n", FAILURES);
}
