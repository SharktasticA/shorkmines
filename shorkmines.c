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



WINDOW *footer_win;
WINDOW *game_win;
WINDOW *header_win;
struct tm_options global_options;
WINDOW *status_win;



void game_loop(WINDOW *window, struct minesweeper_game *game, struct tm_options options)
{
	// Wait for keygame input
	keypad(stdscr, TRUE);
	nodelay(stdscr, FALSE);

	while(1)
	{
		struct minesweeper_tile *previous_tile = game->selected_tile;
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
				tm_move_cursor(game, LEFT, options);
				break;

			case KEY_RIGHT:
			case 'l':
			case 'd':
				tm_move_cursor(game, RIGHT, options);
				break;

			case KEY_UP:
			case 'k':
			case 'w':
				tm_move_cursor(game, UP, options);
				break;

			case KEY_DOWN:
			case 'j':
			case 's':
				tm_move_cursor(game, DOWN, options);
				break;

			case 'g':
			case 'f':
				minesweeper_toggle_flag(game, game->selected_tile);
				update_status_window(status_win, game);
				break;
			/*case ' ':
				minesweeper_space_tile(game, game->selected_tile);
				update_status_window(status_win, game);
				break;*/
			case ',':
			case ' ':
				minesweeper_open_tile(game, game->selected_tile);
				update_status_window(status_win, game);
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

		if (game->selected_tile != previous_tile)
		{
			render_tile(game, previous_tile, window);
			render_tile(game, game->selected_tile, window);
		}

		wrefresh(window);
	}

	render_game(game, window);
	wrefresh(window);



	// Update the status window to display a win/lose message, and then exit
	// the game after a key press
	char *end_text = NULL;
	int end_color = COLOR_PAIR_DEFAULT;
	if (game->state == MINESWEEPER_GAME_OVER)
	{
		end_text = ":-(";
		end_color = COLOR_PAIR_LOSE;
	}
	else if (game->state == MINESWEEPER_WIN)
	{
		end_text = "B-)";
		end_color = COLOR_PAIR_WIN;
	}

	wclear(status_win);
#ifdef EMBEDDED
	draw_box(status_win);
#else
	box(status_win, 0, 0);
#endif

	int win_width, win_height;
	getmaxyx(status_win, win_height, win_width);
	wattron(status_win, COLOR_PAIR(end_color));
	mvwhline(status_win, 1, 1, ' ', win_width - 2);
	wattroff(status_win, COLOR_PAIR(end_color));

	int text_x = (win_width - (int)strlen(end_text)) / 2;
	if (text_x < 1)
		text_x = 1;

	wattron(status_win, COLOR_PAIR(end_color) | A_BOLD);
	mvwprintw(status_win, 1, text_x, "%s", end_text);
	wattroff(status_win, COLOR_PAIR(end_color) | A_BOLD);
	wrefresh(status_win);

	wgetch(window);
#ifdef EMBEDDED
	clear();
	refresh();
#endif
 	endwin();
}

void setup_ncurses()
{
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	cbreak();
	noecho();
	init_colors();
	curs_set(0);
	refresh();
}

void start_with_game(struct minesweeper_game *game, struct tm_options options)
{
	int screen_width, screen_height;
	getmaxyx(stdscr, screen_height, screen_width);

	// Create a header for displaying program name and game size and a footer
	// for game controls
	header_win = newwin(1, screen_width, 0, 0);
	footer_win = newwin(1, screen_width, screen_height - 1, 0);

	// Create window where we will draw the game. Add 2
	// tiles padding so we can draw a box around it
	int window_width = game->width + 2;
	int window_height = game->height + 2;
	int window_x = screen_width / 2 - window_width / 2;
	int window_y = (screen_height - (window_height + 3)) / 2;
	game_win = newwin(window_height, window_width, window_y, window_x);

#ifdef EMBEDDED
	draw_box(game_win);
#else
	box(game_win, 0, 0);
#endif

	status_win = newwin(3, window_width, window_y + window_height, window_x);

	minesweeper_set_cursor(game, game->width / 2, game->height / 2);

	// Draw header and footer
	char title[80];
	snprintf(title, 80, "SHORKMINES - %dx%d", game->width, game->height);
	render_bar(header_win, title);
	render_bar(footer_win, "[hjkl] Move [f] Flag [Space] Open [q] Quit");

	// Draw an initial representation so you see the window when the game starts
	render_game(game, game_win);
	wrefresh(game_win);
	update_status_window(status_win, game);
	wrefresh(status_win);
	
	game_loop(game_win, game, options);
}

// Callback from libminesweeper
void tile_changed(struct minesweeper_game *game, struct minesweeper_tile *tile, void *context)
{
	render_tile(game, tile, game_win);
}

void tm_move_cursor(struct minesweeper_game *game, enum direction direction, struct tm_options options)
{
	minesweeper_move_cursor(game, direction, true);
}
