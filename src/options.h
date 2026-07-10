#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdint.h>



struct SM_OPTIONS {
	int width;
	int height;
	float mineDensity;
};



struct SM_OPTIONS parseOptions(int argc, char **argv);
void showInBuiltHelp(void);
void showManHelp(void);
void showVersion(void);

#endif
