#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdbool.h>
#include <stdint.h>



struct SM_OPTIONS {
	// Requested game board width
	int width;
	// Requested game board height
	int height;
	// Requested mine density (0.0 to 1.0)
	float mineDensity;
	// Flags if mineDensity has been manually set (needed in place of checking
	// against DEFAULT_DENSITY due to rounding difficulties)
	bool mineDensitySet;
};

#define DEFAULT_WIDTH	20
#define DEFAULT_HEIGHT	12
#define DEFAULT_DENSITY	0.15f



struct SM_OPTIONS parseOptions(int, char**);
void showInBuiltHelp(void);
void showManHelp(void);
void showVersion(void);

#endif
