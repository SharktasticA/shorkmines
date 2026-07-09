#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdint.h>

struct tm_options {
	int width;
	int height;
	float mine_density;
};

void show_man_help(void);
void show_inbuilt_help(void);
void show_version(void);
struct tm_options parse_options(int argc, char **argv);

#endif
