#ifndef MAIN_H
#define MAIN_H

#include "node.h"
#include "libhorns.h"

extern int NOTHING_TO_DO;

extern mode_type MODE;

extern int SUCCESSES;
extern int FAILURES;

extern int yy_scan_string(char *);
extern int yyparse(void);

void usage(char *program);
char *banner();
int main(int argc, char **argv);
#endif
