
#include <stdio.h>
#include "verbose.h" 

int QUIET = 0;

void std_out(char *txt) {
	if (QUIET == 1)
		return;
	fprintf(stdout, "%s", txt);
	return;
}

void std_err(char *txt) {
	if (QUIET == 1)
		return;
	fprintf(stderr, "%s", txt);
	return;
}
