#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

struct tm_options {
	int width;
	int height;
	float mine_density;
};

void print_version(void);
void show_help(void);
struct tm_options parse_options(int argc, char **argv);

#endif
