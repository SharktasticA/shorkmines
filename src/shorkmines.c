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
#include "shorkmines.h"

#include <minesweeper.h>
#include <locale.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>



WINDOW *FOOTER_WIN;
WINDOW *GAME_WIN;
WINDOW *HEADER_WIN;
struct SM_OPTIONS OPTIONS;
WINDOW *STATUS_WIN;



void gameLoop(WINDOW *window, struct minesweeper_game *game, struct SM_OPTIONS options)
{
	// Wait for keygame input
	keypad(stdscr, TRUE);
	nodelay(stdscr, FALSE);

	while(1)
	{
		struct minesweeper_tile *prevTile = game->selected_tile;
		int ch = getch();

		if (ch == 27)
		{
			nodelay(stdscr, TRUE);
			int c1 = getch();
			int c2 = ERR;
			if (c1 == '[' || c1 == 'O')
				c2 = getch();
			nodelay(stdscr, FALSE);

			if (c1 == '[' || c1 == 'O')
			{
				switch (c2)
				{
					case 'A':
						ch = KEY_UP;
						break;
					case 'B':
						ch = KEY_DOWN;
						break;
					case 'C':
						ch = KEY_RIGHT;
						break;
					case 'D':
						ch = KEY_LEFT;
						break;
					case 'x':
						ch = KEY_UP;
						break;
					case 'r':
						ch = KEY_DOWN;
						break;
					case 't':
						ch = KEY_LEFT;
						break;
					case 'v':
						ch = KEY_RIGHT;
						break;
					default:
						ch = 27;
						break;
				}
			}
		}

		if (ch == KEY_F(1))
			break;
		if (!(game->state == MINESWEEPER_PENDING_START || game->state == MINESWEEPER_PLAYING))
			break;

		switch(ch)
		{
			case KEY_LEFT:
			case 'h':
			case 'a':
				tmMoveCursor(game, LEFT, options);
				break;

			case KEY_RIGHT:
			case 'l':
			case 'd':
				tmMoveCursor(game, RIGHT, options);
				break;

			case KEY_UP:
			case 'k':
			case 'w':
				tmMoveCursor(game, UP, options);
				break;

			case KEY_DOWN:
			case 'j':
			case 's':
				tmMoveCursor(game, DOWN, options);
				break;

			case 'g':
			case 'f':
				minesweeper_toggle_flag(game, game->selected_tile);
				updateStatusWindow(STATUS_WIN, game);
				break;
			/*case ' ':
				minesweeper_space_tile(game, game->selected_tile);
				updateStatusWindow(STATUS_WIN, game);
				break;*/
			case ',':
			case ' ':
				minesweeper_open_tile(game, game->selected_tile);
				updateStatusWindow(STATUS_WIN, game);
				break;
			case 'q':
#ifdef EMBEDDED
				clear();
				refresh();
#endif
				endwin();
				exit(0);
				break;
		}

		if (game->selected_tile != prevTile)
		{
			renderTile(game, prevTile, window);
			renderTile(game, game->selected_tile, window);
		}

		wrefresh(window);
	}

	renderGame(game, window);
	wrefresh(window);



	// Update the status window to display a win/lose message, and then exit
	// the game after a key press
	char *endText = NULL;
	int endColour = COLOR_PAIR_DEFAULT;
	if (game->state == MINESWEEPER_GAME_OVER)
	{
		endText = ":-(";
		endColour = COLOR_PAIR_LOSE;
	}
	else if (game->state == MINESWEEPER_WIN)
	{
		endText = "B-)";
		endColour = COLOR_PAIR_WIN;
	}

	wclear(STATUS_WIN);
#ifdef EMBEDDED
	drawBox(STATUS_WIN);
#else
	box(STATUS_WIN, 0, 0);
#endif

	int winWidth, winHeight;
	getmaxyx(STATUS_WIN, winHeight, winWidth);
	wattron(STATUS_WIN, COLOR_PAIR(endColour));
	mvwhline(STATUS_WIN, 1, 1, ' ', winWidth - 2);
	wattroff(STATUS_WIN, COLOR_PAIR(endColour));

	int textX = (winWidth - (int)strlen(endText)) / 2;
	if (textX < 1)
		textX = 1;

	wattron(STATUS_WIN, COLOR_PAIR(endColour) | A_BOLD);
	mvwprintw(STATUS_WIN, 1, textX, "%s", endText);
	wattroff(STATUS_WIN, COLOR_PAIR(endColour) | A_BOLD);
	wrefresh(STATUS_WIN);

	wgetch(window);
#ifdef EMBEDDED
	clear();
	refresh();
#endif
 	endwin();
}

void setupNcurses()
{
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	cbreak();
	noecho();
	initColours();
	curs_set(0);
	refresh();
}

void startWithGame(struct minesweeper_game *game, struct SM_OPTIONS options)
{
	int screenWidth, screenHeight;
	getmaxyx(stdscr, screenHeight, screenWidth);

	// Create a header for displaying program name and game size and a footer
	// for game controls
	HEADER_WIN = newwin(1, screenWidth, 0, 0);
	FOOTER_WIN = newwin(1, screenWidth, screenHeight - 1, 0);

	// Create window where we will draw the game. Add 2
	// tiles padding so we can draw a box around it
	int windowWidth = game->width + 2;
	int windowHeight = game->height + 2;
	int windowX = screenWidth / 2 - windowWidth / 2;
	int windowY = (screenHeight - (windowHeight + 3)) / 2;
	GAME_WIN = newwin(windowHeight, windowWidth, windowY, windowX);

#ifdef EMBEDDED
	drawBox(GAME_WIN);
#else
	box(GAME_WIN, 0, 0);
#endif

	STATUS_WIN = newwin(3, windowWidth, windowY + windowHeight, windowX);

	minesweeper_set_cursor(game, game->width / 2, game->height / 2);

	// Draw header and footer
	char title[80];
	snprintf(title, 80, "SHORKMINES - %dx%d", game->width, game->height);
	renderBar(HEADER_WIN, title);
	renderBar(FOOTER_WIN, "[hjkl] Move [f] Flag [Space] Open [q] Quit");

	// Draw an initial representation so you see the window when the game starts
	renderGame(game, GAME_WIN);
	wrefresh(GAME_WIN);
	updateStatusWindow(STATUS_WIN, game);
	wrefresh(STATUS_WIN);
	
	gameLoop(GAME_WIN, game, options);
}

// Callback from libminesweeper
void tileChanged(struct minesweeper_game *game, struct minesweeper_tile *tile, void *context)
{
	renderTile(game, tile, GAME_WIN);
}

void tmMoveCursor(struct minesweeper_game *game, enum direction direction, struct SM_OPTIONS options)
{
	minesweeper_move_cursor(game, direction, true);
}
