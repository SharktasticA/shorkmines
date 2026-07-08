#include "graphics.h"
#include <string.h>

extern struct tm_options global_options;


#ifdef EMBEDDED

void draw_box(WINDOW *win)
{
	int height, width;
	getmaxyx(win, height, width);

	mvwhline(win, 0, 1, '-', width - 2);
	mvwhline(win, height - 1, 1, '-', width - 2);
	mvwvline(win, 1, 0, '|', height - 2);
	mvwvline(win, 1, width - 1, '|', height - 2);

	mvwaddch(win, 0, 0, '+');
	mvwaddch(win, 0, width - 1, '+');
	mvwaddch(win, height - 1, 0, '+');
	mvwaddch(win, height - 1, width - 1, '+');
}

#endif

void init_colors()
{
	use_default_colors();
	init_pair(COLOR_PAIR_DEFAULT, -1, -1);
	init_pair(COLOR_PAIR_CURSOR, -1, COLOR_GREEN);
	init_pair(COLOR_PAIR_MINE, -1, COLOR_RED);
	init_pair(COLOR_PAIR_FLAG, COLOR_YELLOW, -1);
	init_pair(COLOR_PAIR_1, COLOR_BLUE, -1);
	init_pair(COLOR_PAIR_2, COLOR_GREEN, -1);
	init_pair(COLOR_PAIR_3, COLOR_RED, -1);
	init_pair(COLOR_PAIR_4, COLOR_YELLOW, -1);
	// TODO: Set colors for 5,6,7,8
	init_pair(COLOR_PAIR_BAR, COLOR_WHITE, COLOR_BLUE);
}

void render_bar(WINDOW *window, const char *text)
{
	wbkgd(window, COLOR_PAIR(COLOR_PAIR_BAR) | ' ');
	werase(window);
	wattron(window, COLOR_PAIR(COLOR_PAIR_BAR) | A_BOLD);
	mvwprintw(window, 0, 1, "%s", text);
	wattroff(window, COLOR_PAIR(COLOR_PAIR_BAR) | A_BOLD);
	wrefresh(window);
}

void render_game(struct minesweeper_game *game, WINDOW *window)
{
	for (unsigned x = 0; x < game->width; x++) 
	{
		for (unsigned y = 0; y < game->height; y++) 
		{
			struct minesweeper_tile *tile = minesweeper_get_tile_at(game, x, y);
			render_tile(game, tile, window);
		}
	}
}

#ifdef EMBEDDED
void draw_box(WINDOW *window);
#endif
void render_tile(struct minesweeper_game *game, struct minesweeper_tile *tile, WINDOW *window)
{
	int index = tile_index_for_tile(game, tile);
	bool is_cursor = tile == game->selected_tile;
	if (index == TILE_INDEX_MINE)
		wattron(window, COLOR_PAIR(COLOR_PAIR_MINE));
	else if (is_cursor)
		wattron(window, COLOR_PAIR(COLOR_PAIR_CURSOR));
	else if (index == TILE_INDEX_FLAG)
		wattron(window, COLOR_PAIR(COLOR_PAIR_FLAG));
	else if (index >= 1 && index <= 8)
		wattron(window, COLOR_PAIR(COLOR_PAIR_1 + (index - 1)));
	else
		wattron(window, COLOR_PAIR(COLOR_PAIR_DEFAULT));

	// Add 1 to the position in both axes so we stay within the
	// window border.
	unsigned x, y; minesweeper_get_tile_location(game, tile, &x, &y);
	static char *tile_map[] = TILE_MAP;
	mvwaddstr(window, y + 1, x + 1, tile_map[index]);
}

int tile_index_for_tile(struct minesweeper_game *game, struct minesweeper_tile *tile)
{
	// Opened with mine, and game over state where all mines are displayed
	if ((tile->has_mine && game->state == MINESWEEPER_GAME_OVER) || (tile->has_mine && tile->is_opened))
		return TILE_INDEX_MINE;

	// On opened tile, we show the adjacent mine count
	if (tile->is_opened)
		return tile->adjacent_mine_count;

	// Flags are shown as an 'F'
	if (tile->has_flag)
		return TILE_INDEX_FLAG;

	// Unopened tiles
	return TILE_INDEX_UNOPENED;
}

void update_status_window(WINDOW *status_window, struct minesweeper_game *game)
{
	int window_width = game->width;
	
	char mine_text[128];
	char flag_text[128];

	if (window_width >= 20)
	{
		snprintf(mine_text, sizeof(mine_text), "Mines: %d", game->mine_count);
		snprintf(flag_text, sizeof(flag_text), "Flags: %d", game->flag_count);
	}
	else
	{
		snprintf(mine_text, sizeof(mine_text), "M: %d", game->mine_count);
		snprintf(flag_text, sizeof(flag_text), "F: %d", game->flag_count);
	}

	int flag_text_length = strlen(flag_text);

#ifdef EMBEDDED
	draw_box(status_window);
#else
	box(status_window, 0, 0);
#endif
	mvwprintw(status_window, 1, 1, "%s", mine_text);
	mvwprintw(status_window, 1, window_width - (flag_text_length - 1), "%s", flag_text);
	wrefresh(status_window);
}
