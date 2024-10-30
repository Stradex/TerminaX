#ifndef GAME_H
#define GAME_H

#include <geometry.h>
#include <terminal.h>

#define TEXT_COLOR 		1
#define BORDER_COLOR 	2

typedef struct {
	Terminal t;
	Rect screen;
	int status;
} Game;


void init_colors(void);
void init_ncurses(void);
void refresh_game_size(Game* game);
Game* init_game(void);
void game_render(Game* game, int ch);
void game_loop(Game* game);
void game_end(Game* game);

#endif
