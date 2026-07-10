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



extern WINDOW *FOOTER_WIN;
extern WINDOW *GAME_WIN;
extern WINDOW *HEADER_WIN;
extern struct SM_OPTIONS OPTIONS;
extern WINDOW *STATUS_WIN;



void gameLoop(WINDOW*, struct minesweeper_game*, struct SM_OPTIONS);
void setupNcurses();
void startWithGame(struct minesweeper_game*, struct SM_OPTIONS);
void tileChanged(struct minesweeper_game*, struct minesweeper_tile*, void*);
void tmMoveCursor(struct minesweeper_game*, enum direction, struct SM_OPTIONS);
