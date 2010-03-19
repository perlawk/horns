#ifndef TEST_H
#define TEST_H

#include "node.h"
#include "libhorns.h"

extern node *TAIL;
extern node *NODE_RESULT;
extern int NOTHING_TO_DO;
extern mode_type MODE;
extern int SUCCESSES;
extern int FAILURES;

extern int yy_scan_string(char *);
extern int yyparse(void);

void test_code(char *code, node *result);
void test();

#endif