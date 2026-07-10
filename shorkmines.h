/*
    ######################################################
    ##         SHORK ENTERTAINMENT - SHORKMINES         ##
    ######################################################
    ## Main program logic                               ##
    ######################################################
    ## Based on terminal-mines by Joel Ekström          ##
    ######################################################
    ## Licence: MIT                                     ##
    ######################################################
    ## Kali (links.sharktastica.co.uk)                  ##
    ######################################################
*/



#include "graphics.h"
#include "options.h"

#include <minesweeper.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>



extern WINDOW *footer_win;
extern WINDOW *game_win;
extern WINDOW *header_win;
extern struct tm_options global_options;
extern WINDOW *status_win;



void game_loop(WINDOW*, struct minesweeper_game*, struct tm_options);
void setup_ncurses();
void start_with_game(struct minesweeper_game*, struct tm_options);
void tile_changed(struct minesweeper_game*, struct minesweeper_tile*, void*);
void tm_move_cursor(struct minesweeper_game*, enum direction, struct tm_options);
