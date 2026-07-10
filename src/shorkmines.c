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
#include "shorkmenu.h"
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
				moveCursor(game, LEFT, options);
				break;

			case KEY_RIGHT:
			case 'l':
			case 'd':
				moveCursor(game, RIGHT, options);
				break;

			case KEY_UP:
			case 'k':
			case 'w':
				moveCursor(game, UP, options);
				break;

			case KEY_DOWN:
			case 'j':
			case 's':
				moveCursor(game, DOWN, options);
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

	// Update footer and way for 'q' before exiting
	renderBar(FOOTER_WIN, "[q] Quit");
	int endCh;
	do
	{
		endCh = wgetch(window);
	} while (endCh != 'q');
#ifdef EMBEDDED
	clear();
	refresh();
#endif
 	endwin();
}

void moveCursor(struct minesweeper_game *game, enum direction direction, struct SM_OPTIONS options)
{
	minesweeper_move_cursor(game, direction, true);
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

/**
 * 
 * @return 1 if game should launch after menu; 0 if not
 */
int showMainMenu(void)
{
	setupMenuSys();


	MenuItem menu[] = {
		{ "",      "Select size & difficulty", NULL, NULL, 1, 1 },
		{ "def",   "Default       (20x12, 15\% mines)", NULL, NULL, 1, 0 },
		{ "full",  "", NULL, NULL, 1, 0 },
		{ "diff1", "Beginner      (9x9, 13\% mines)", NULL, NULL, 1, 0 },
		{ "diff2", "Intermediate  (16x16, 17\% mines)", NULL, NULL, 1, 0 },
		{ "diff3", "Expert        (30x16, 25\% mines)", NULL, NULL, 1, 0 },
		{ "diff4", "Impossible    (42x16, 50\% mines)", NULL, NULL, 1, 0 }
	};
	int fullScreenWidth = TERM_SIZE.ws_col - 2;
	int fullScreenHeight = TERM_SIZE.ws_row - 7;
	snprintf(menu[2].name, 80, "Full screen   (%dx%d, 15%% mines)", fullScreenWidth, fullScreenHeight);
	int menuSize = sizeof(menu) / sizeof(menu[0]);

	int result = 1;
	int running = 1;
	int cursorX = 1;
	int cursorY = 1;
	int cursorXPrev = 1;
	int cursorYPrev = 0;
	int fullRedraw = 1;

	while (running)
	{
		if (fullRedraw)
		{
			clearScreen();
			printHeader("SHORKMINES");
			printMenu(menu, menuSize, NULL, 1, TERM_SIZE.ws_col - 6, menuSize, &cursorX, &cursorY, &cursorXPrev, &cursorYPrev);
			printFooter("[jk] Navigate [Enter] Select [q] Quit");
		}
		else
		{
			if (COL_ENABLED)
				printf("\x1b[2;1H");
			else
				printf("\x1b[3;1H");
			printMenu(menu, menuSize, NULL, 1, TERM_SIZE.ws_col - 6, menuSize, &cursorX, &cursorY, &cursorXPrev, &cursorYPrev);
		}

		NavInput input = getNavInput();

		fullRedraw = 1;
		cursorYPrev = 0;
		switch (input)
		{
			case CURSOR_UP:
				cursorYPrev = cursorY;
				cursorY--;
				if (cursorY < 1) cursorY = menuSize;
				fullRedraw = 0;
				break;

			case CURSOR_DOWN:
				cursorYPrev = cursorY;
				cursorY++;
				if (cursorY > menuSize) cursorY = 1;
				fullRedraw = 0;
				break;

			case ENTER:
				running = 0;
				if (strcmp(menu[cursorY - 1].id, "def") == 0)
				{
					// Don't need to do anything!
				}
				else if (strcmp(menu[cursorY - 1].id, "full") == 0)
				{
					OPTIONS.width = fullScreenWidth;
					OPTIONS.height = fullScreenHeight;
					OPTIONS.mineDensity = 0.15;
				}
				else if (strcmp(menu[cursorY - 1].id, "diff1") == 0)
				{
					OPTIONS.width = 9;
					OPTIONS.height = 9;
					OPTIONS.mineDensity = 0.13;
				}
				else if (strcmp(menu[cursorY - 1].id, "diff2") == 0)
				{
					OPTIONS.width = 16;
					OPTIONS.height = 16;
					OPTIONS.mineDensity = 0.17;
				}
				else if (strcmp(menu[cursorY - 1].id, "diff3") == 0)
				{
					OPTIONS.width = 30;
					OPTIONS.height = 16;
					OPTIONS.mineDensity = 0.25;
				}
				else if (strcmp(menu[cursorY - 1].id, "diff4") == 0)
				{
					OPTIONS.width = 42;
					OPTIONS.height = 16;
					OPTIONS.mineDensity = 0.5;
				}
				break;
		
			case QUIT:
				running = 0;
				result = 0;
				break;

			case INVALID:
				fullRedraw = 0;
				break;

			default:
				continue;
		}
	}

	freeMenu(menu, menuSize);
	onExit();
	return result;
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
