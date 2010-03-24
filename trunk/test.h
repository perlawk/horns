#ifndef TEST_H
#define TEST_H

#include "node.h"

extern int yy_scan_string(char *);

void test_code(char *code, node *result);
void test();
#endif
