#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#ifndef __MINGW32__
	#include <readline/readline.h>
	#include <readline/history.h>
#endif

#include "node.h"
#include "test.h"
#include "main.h"

extern FILE *yyin;

extern node *TAIL;
extern node *NODE_RESULT;

int NOTHING_TO_DO;

mode_type MODE;

int SUCCESSES;
int FAILURES;

extern int yy_scan_string(char *);
extern int yyparse(void);

void usage(char *program) {
	printf("Usage: %s [script]\n", program);
	printf("-e <exp> evaluate\n");
	printf("-h help\n");
	printf("-t test\n");
	printf("-v version\n");
	exit(0);
}

char *banner() {
	char *result=alloc_string(strlen(VERSION)+strlen(ARCH)+strlen(COPYRIGHT)+6);
	sprintf(result, "Horns v%s %s %s", VERSION, ARCH, COPYRIGHT);
	return result;
}

int main(int argc, char **argv) {
	horns_init();

	NOTHING_TO_DO=0;

	MODE=INTERACTIVE_MODE;

	SUCCESSES=0;
	FAILURES=0;

	char *line=alloc_string(MAX_LINE);

	char *script_name="", *exp="";
	FILE *script;

	opterr=0;

	char c;

	while ((c=getopt(argc, argv, "htve:")) != -1) {
		switch(c) {
			case 'h':
				MODE=HELP_MODE;
				break;
			case 't':
				MODE=TEST_MODE;
				break;
			case 'v':
				MODE=VERSION_MODE;
				break;
			case 'e':
				MODE=EXP_MODE;
				exp=optarg;
				break;
			case '?':
				if (optopt == 'e') usage(argv[0]);
		}
	}

	if (optind < argc) {
		MODE=SCRIPT_MODE;
		script_name=argv[optind++];
	}

	node *ARGV=node_empty_list();

	int i;
	for (i=optind; i < argc; i++) {
		ARGV=node_append(ARGV, node_str(argv[i]));
	}

	node_set(node_sym("\'ARGV"), ARGV);

	switch(MODE) {
		case HELP_MODE:
			usage(argv[0]);
		case VERSION_MODE:
			printf("%s\n", banner());
			exit(0);
		case TEST_MODE:
			test();
		case EXP_MODE:
			yy_scan_string(exp);
			yyparse();

			if (!NOTHING_TO_DO) printf("%s\n", node_string(NODE_RESULT, 1));

			break;
		case INTERACTIVE_MODE:
			printf("%s\n", banner());
			printf("%s\n", HOW_TO_EXIT);

			#ifdef __MINGW32__
			printf("%s", PROMPT);
			while ((line=fgets(line, MAX_LINE, stdin)) != NULL) {
			#else
			while ((line=readline(PROMPT)) != NULL) {
				if (strlen(line)>0) add_history(line);
			#endif

				NOTHING_TO_DO=0;

				if (!streq(line, "\n")) {
					yy_scan_string(line);
					yyparse();

					if (!NOTHING_TO_DO) printf("=>%s\n", node_string(NODE_RESULT, 1));
				}

				#ifdef __MINGW32__
				printf("%s", PROMPT);
				#endif
			}

			break;
		case SCRIPT_MODE:
			script=fopen(script_name, "r");
			if (!script) {
				printf("Could not open %s\n", script_name);
				exit(0);
			}

			yyin=script;
			yyparse();
			fclose(yyin);
	}

	return 0;
}
