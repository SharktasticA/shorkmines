#include "general.h"
#include "options.h"

#include <getopt.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



static const char *VERSION = "1.3-wip";



struct SM_OPTIONS parseOptions(int argc, char **argv)
{
	static struct option options[] = {
		{ "width", required_argument, NULL, 'w' },
		{ "height", required_argument, NULL, 'h' },
		{ "mine-density",  required_argument, NULL, 'm' },
		{ "help", no_argument, NULL, 1 },
		{ "version", no_argument, NULL, 'v' },
		{ NULL, 0, NULL, 0 }
	};

	// Default options
	struct SM_OPTIONS SM_OPTIONS = {
		.width = DEFAULT_WIDTH,
		.height = DEFAULT_HEIGHT,
		.mineDensity = DEFAULT_DENSITY,
		.mineDensitySet = false
	};

	signed char param;
	while ((param = getopt_long(argc, argv, "w:h:m:v", options, NULL)) != -1)
	{
		switch (param)
		{
			case 'w':
			{
				uintmax_t value = strtoumax(optarg, NULL, 10);
				if (value != UINTMAX_MAX)
					SM_OPTIONS.width = (int)value;
				break;
			}

			case 'h':
			{
				uintmax_t value = strtoumax(optarg, NULL, 10);
				if (value != UINTMAX_MAX)
					SM_OPTIONS.height = (int)value;
				break;
			}

			case 'm':
			{
				float value = strtof(optarg, NULL);
				if (value != 0)
				{
					SM_OPTIONS.mineDensity = value;
					SM_OPTIONS.mineDensitySet = true;
				}
				break;
			}

			case 'v':
			{
				showVersion();
				exit(0);
			}

			case 1:
			{
				showManHelp();
				exit(0);
			}
		}
	}
	
	return SM_OPTIONS;
}

void showInBuiltHelp()
{
	struct winsize termSize = getTerminalSize();

    char desc[100] = "An ncurses-based minesweeper game played using a keyboard instead of a mouse.\n";
    formatNewLines(desc, termSize.ws_col, NULL, 0);
    printf("%s\n", desc);

    char usage[60] = "Usage: shorkmines [OPTIONS]\n\n";
    formatNewLines(usage, termSize.ws_col, NULL, 0);
    printf("%s", usage);

    char options[20] = "Options:\n";
    formatNewLines(options, termSize.ws_col, NULL, 0);
    printf("%s", options);

    char height[100] = "-h, --height        Specifies the game board's height (will be clamped if taller than window)\n";
    formatNewLines(height, termSize.ws_col, "                    ", 0);
    printf("%s", height);

    char help[140] = "--help              Shows a manual page (if present) instead of launching or displays in-built help information and exits\n";
    formatNewLines(help, termSize.ws_col, "                    ", 0);
    printf("%s", help);

    char mineDensity[140] = "-m, --mine-density  Specifies how many mines will be placed on the game board (from 0.0 for no mines to 1.0 for all mines)\n";
    formatNewLines(mineDensity, termSize.ws_col, "                    ", 0);
    printf("%s", mineDensity);

    char width[100] = "-w, --width         Specifies the game board's width (will be clamped if wider than window)\n";
    formatNewLines(width, termSize.ws_col, "                    ", 0);
    printf("%s", width);

    char version[100] = "-v, --version       Displays version number and exits\n\n";
    formatNewLines(version, termSize.ws_col, "                    ", 0);
    printf("%s", version);

    char controls[20] = "Controls:\n";
    formatNewLines(controls, termSize.ws_col, NULL, 0);
    printf("%s", controls);

    char hjkl[100] = "hjkl, wasd, arrow keys  Moves the cursor to select a tile\n";
    formatNewLines(hjkl, termSize.ws_col, "                        ", 0);
    printf("%s", hjkl);

    char f[100] = "f, g                    Places or removes a flag at the selected tile\n";
    formatNewLines(f, termSize.ws_col, "                        ", 0);
    printf("%s", f);

    char space[100] = "Space, ,                Opens the selected tile\n";
    formatNewLines(space, termSize.ws_col, "                        ", 0);
    printf("%s", space);
}

void showManHelp()
{
	// If man is not found, call to show the inbuilt help
	if (!isProgramInstalled("man", true))
	{
		showInBuiltHelp();
		return;
	}

	// If manpage exists locally, prioritize it to allow local builds to show help
	char *manPagePath = NULL;
	if (access("man/shorkmines.1", F_OK | R_OK) != -1)
		manPagePath = "man/shorkmines.1";
	else
		manPagePath = "shorkmines";

	pid_t pid = fork();
	if (pid == 0) /* Child process */
	{
		// Suppress man's "no manual entry" message since we have a fallback
		freopen("/dev/null", "w", stderr);

		char *argv[] = {"man", manPagePath, NULL};
		execv("/usr/bin/man", argv);
		exit(127); /* only if execv fails */
	}
	else /* pid != 0; parent process */
	{
		int status;
		waitpid(pid, &status, 0); /* wait for child to exit */

		// If unsuccessful - say issue with man, manpage not found - resort to
		// in-built fallback help
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			showInBuiltHelp();
	}
}

void showVersion()
{
	printf("SHORKMINES %s\n", VERSION);
}
