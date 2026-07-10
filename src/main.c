/*
    ######################################################
    ##         SHORK ENTERTAINMENT - SHORKMINES         ##
    ######################################################
    ## An ncurses-based minesweeper game played using a ##
    ## keyboard instead of a mouse                      ##
    ######################################################
    ## Based on terminal-mines by Joel Ekström          ##
    ######################################################
    ## Licence: MIT                                     ##
    ######################################################
    ## Kali (links.sharktastica.co.uk)                  ##
    ######################################################
*/



#include "shorkmines.h"



int main(int argc, char **argv)
{
	OPTIONS = parseOptions(argc, argv);

	if (OPTIONS.width == DEFAULT_WIDTH && OPTIONS.height == DEFAULT_HEIGHT && !OPTIONS.mineDensitySet)
		if (!showMainMenu())
			return 0;

	setupNcurses();

	// Make sure the desired size is not larger than the terminal window
	int screenWidth, screenHeight;
	getmaxyx(stdscr, screenHeight, screenWidth);

	// -2 for left and right borders
	int maxWidth = screenWidth - 2; 
	// -2 for top and bottom borders, -3 for status window, -2 for extra
	// padding
	int maxHeight = screenHeight - 7;

	if (OPTIONS.width > maxWidth) 
		OPTIONS.width = maxWidth;

	if (OPTIONS.height > maxHeight)
		OPTIONS.height = maxHeight;

	// Set up a game game
	srand(time(NULL));
	uint8_t *buffer = malloc(minesweeper_minimum_buffer_size(OPTIONS.width, OPTIONS.height));
	struct minesweeper_game *game = minesweeper_init(OPTIONS.width, OPTIONS.height, OPTIONS.mineDensity, buffer);
	game->tile_update_callback = &tileChanged;

	// Start the ncurses frontend
	startWithGame(game, OPTIONS);
	free(buffer);
}
