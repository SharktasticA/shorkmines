#include <getopt.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "options.h"



static const char *VERSION = "1.3-wip";



struct tm_options parse_options(int argc, char **argv)
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
	struct tm_options tm_options = {
		.width = 20,
		.height = 12,
		.mine_density = 0.1
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
					tm_options.width = (int)value;
				break;
			}

			case 'h':
			{
				uintmax_t value = strtoumax(optarg, NULL, 10);
				if (value != UINTMAX_MAX)
					tm_options.height = (int)value;
				break;
			}

			case 'm':
			{
				float value = strtof(optarg, NULL);
				if (value != 0)
					tm_options.mine_density = value;
				break;
			}

			case 'v':
			{
				print_version();
				exit(0);
			}

			case 1:
			{
				show_help();
				exit(0);
			}
		}
	}
	
	return tm_options;
}

void show_help()
{
	// If manpage exists locally, prioritize it to allow local builds to show help
	char *manpage_path = NULL;
	if (access("man/shorkmines.1", F_OK | R_OK) != -1 )
		manpage_path = "man/shorkmines.1";
	else
		manpage_path = "shorkmines";

	pid_t pid = fork();
	if (pid == 0) /* Child process */
	{
		char *argv[] = {"man", manpage_path, NULL};
		execv("/usr/bin/man", argv);
		exit(127); /* only if execv fails */
	}
	else /* pid != 0; parent process */
		waitpid(pid, 0, 0); /* wait for child to exit */
}

void print_version()
{
	printf("SHORKMINES %s\n", VERSION);
}
