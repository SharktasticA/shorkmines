#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdint.h>



struct SM_OPTIONS {
	int width;
	int height;
	float mineDensity;
};

#define DEFAULT_WIDTH	20
#define DEFAULT_HEIGHT	12
#define DEFAULT_DENSITY 0.15f



struct SM_OPTIONS parseOptions(int, char**);
void showInBuiltHelp(void);
void showManHelp(void);
void showVersion(void);

#endif
