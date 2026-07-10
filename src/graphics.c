#include "graphics.h"
#include <string.h>

extern struct SM_OPTIONS OPTIONS;


#ifdef EMBEDDED

void drawBox(WINDOW *win)
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

void initColours()
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
	init_pair(COLOR_PAIR_WIN, COLOR_WHITE, COLOR_GREEN);
	init_pair(COLOR_PAIR_LOSE, COLOR_WHITE, COLOR_RED);
}

void renderBar(WINDOW *window, const char *text)
{
	wbkgd(window, COLOR_PAIR(COLOR_PAIR_BAR) | ' ');
	werase(window);
	wattron(window, COLOR_PAIR(COLOR_PAIR_BAR) | A_BOLD);
	mvwprintw(window, 0, 0, "%s", text);
	wattroff(window, COLOR_PAIR(COLOR_PAIR_BAR) | A_BOLD);
	wrefresh(window);
}

void renderGame(struct minesweeper_game *game, WINDOW *window)
{
	for (unsigned x = 0; x < game->width; x++) 
	{
		for (unsigned y = 0; y < game->height; y++) 
		{
			struct minesweeper_tile *tile = minesweeper_get_tile_at(game, x, y);
			renderTile(game, tile, window);
		}
	}
}

void renderTile(struct minesweeper_game *game, struct minesweeper_tile *tile, WINDOW *window)
{
	int index = tileIndexForTile(game, tile);
	bool isCursor = tile == game->selected_tile;

	if (index == TILE_INDEX_MINE)
		wattron(window, COLOR_PAIR(COLOR_PAIR_MINE) | A_BOLD);
	else if (isCursor)
		wattron(window, COLOR_PAIR(COLOR_PAIR_CURSOR) | A_BOLD);
	else if (index == TILE_INDEX_FLAG)
		wattron(window, COLOR_PAIR(COLOR_PAIR_FLAG) | A_BOLD);
	else if (index >= 1 && index <= 8)
		wattron(window, COLOR_PAIR(COLOR_PAIR_1 + (index - 1)) | A_BOLD);
	else
		wattron(window, COLOR_PAIR(COLOR_PAIR_DEFAULT) | A_BOLD);

	// Add 1 to the position in both axes so we stay within the
	// window border.
	unsigned x, y; minesweeper_get_tile_location(game, tile, &x, &y);
	static char *tileMap[] = TILE_MAP;
	mvwaddstr(window, y + 1, x + 1, tileMap[index]);
}

int tileIndexForTile(struct minesweeper_game *game, struct minesweeper_tile *tile)
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

void updateStatusWindow(WINDOW *STATUS_WINDOW, struct minesweeper_game *game)
{
	int windowWidth = game->width;
	
	char mineText[128];
	char flagText[128];

	if (windowWidth >= 20)
	{
		snprintf(mineText, sizeof(mineText), "Mines: %d", game->mine_count);
		snprintf(flagText, sizeof(flagText), "Flags: %d", game->flag_count);
	}
	else if (windowWidth >= 12)
	{
		snprintf(mineText, sizeof(mineText), "M: %d", game->mine_count);
		snprintf(flagText, sizeof(flagText), "F: %d", game->flag_count);
	}
	else
	{
		snprintf(mineText, sizeof(mineText), "M %d", game->mine_count);
		snprintf(flagText, sizeof(flagText), "F %d", game->flag_count);
	}

	int flagTextLen = strlen(flagText);

#ifdef EMBEDDED
	drawBox(STATUS_WINDOW);
#else
	box(STATUS_WINDOW, 0, 0);
#endif
	mvwprintw(STATUS_WINDOW, 1, 1, "%s", mineText);
	mvwprintw(STATUS_WINDOW, 1, windowWidth - (flagTextLen - 1), "%s", flagText);
	wrefresh(STATUS_WINDOW);
}
