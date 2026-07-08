#include <minesweeper.h>
#include <locale.h>
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "options.h"
#include "graphics.h"

void start_with_game(struct minesweeper_game *game, struct tm_options options);
void game_loop(WINDOW *window, struct minesweeper_game *game, struct tm_options options);
void tile_changed(struct minesweeper_game *game, struct minesweeper_tile *tile, void *context);

WINDOW *game_win;
WINDOW *status_win;
WINDOW *header_win;
WINDOW *footer_win;
struct tm_options global_options;

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

void tm_move_cursor(struct minesweeper_game *game, enum direction direction, struct tm_options options)
{
	minesweeper_move_cursor(game, direction, true);
}

void game_loop(WINDOW *window, struct minesweeper_game *game, struct tm_options options)
{
	// Wait for keygame input
	keypad(stdscr, TRUE);
	int ch;
	while((ch = getch()) != KEY_F(1) && (game->state == MINESWEEPER_PENDING_START || game->state == MINESWEEPER_PLAYING))
	{
		struct minesweeper_tile *previous_tile = game->selected_tile;
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

	// Create a win/lose window, and then exit the game after a key press
	char *end_text = NULL;
	if (game->state == MINESWEEPER_GAME_OVER)
		end_text = "BOOM";
	else if (game->state == MINESWEEPER_WIN)
		end_text = "CONGRATS";

	int screen_width, screen_height;
	getmaxyx(stdscr, screen_height, screen_width);
	int window_width = strlen(end_text) + 2;
	WINDOW *end_win = newwin(3, window_width, screen_height / 2 - 2, screen_width / 2 - window_width / 2);
#ifdef EMBEDDED
	draw_box(end_win);
#else
	box(end_win, 0, 0);
#endif
	mvwprintw(end_win, 1, 1, end_text);
	wrefresh(end_win);

	wgetch(window);
#ifdef EMBEDDED
	clear();
	refresh();
#endif
 	endwin();
}

// Callback from libminesweeper
void tile_changed(struct minesweeper_game *game, struct minesweeper_tile *tile, void *context)
{
	render_tile(game, tile, game_win);
}
