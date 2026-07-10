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
	global_options = parse_options(argc, argv);
	setup_ncurses();

	// Make sure the desired size is not larger than the terminal window
	int screen_width, screen_height;
	getmaxyx(stdscr, screen_height, screen_width);

	// -2 for left and right borders
	int max_width = screen_width - 2; 
	// -2 for top and bottom borders, -3 for status window, -2 for extra
	// padding
	int max_height = screen_height - 7;

	if (global_options.width > max_width) 
		global_options.width = max_width;

	if (global_options.height > max_height)
		global_options.height = max_height;

	// Set up a game game
	srand(time(NULL));
	uint8_t *buffer = malloc(minesweeper_minimum_buffer_size(global_options.width, global_options.height));
	struct minesweeper_game *b = minesweeper_init(global_options.width, global_options.height, global_options.mine_density, buffer);
	b->tile_update_callback = &tile_changed;

	// Start the ncurses frontend
	start_with_game(b, global_options);
	free(buffer);
}
