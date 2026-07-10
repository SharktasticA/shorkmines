#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <ncurses.h>
#include <minesweeper.h>
#include "options.h"

enum {
	COLOR_PAIR_DEFAULT = 1,
	COLOR_PAIR_CURSOR,
	COLOR_PAIR_MINE,
	COLOR_PAIR_FLAG,
	COLOR_PAIR_1,
	COLOR_PAIR_2,
	COLOR_PAIR_3,
	COLOR_PAIR_4,
	COLOR_PAIR_5,
	COLOR_PAIR_6,
	COLOR_PAIR_7,
	COLOR_PAIR_8,
	COLOR_PAIR_BAR,
	COLOR_PAIR_WIN,
	COLOR_PAIR_LOSE
};

#define TILE_MAP {" ", "1", "2", "3", "4", "5", "6", "7", "8", "#", "*", "F"}

enum TILE_INDEX {
    TILE_INDEX_EMPTY = 0,
	TILE_INDEX_UNOPENED = 9,
	TILE_INDEX_MINE,
	TILE_INDEX_FLAG,
};

#ifdef EMBEDDED
void drawBox(WINDOW *window);
#endif
void initColours();
void renderBar(WINDOW *window, const char *text);
void renderGame(struct minesweeper_game *game, WINDOW *window);
void renderTile(struct minesweeper_game *game, struct minesweeper_tile *tile, WINDOW *window);
int tileIndexForTile(struct minesweeper_game *game, struct minesweeper_tile *tile);
void updateStatusWindow(WINDOW *STATUS_WINDOW, struct minesweeper_game *game);

#endif
