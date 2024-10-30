#ifndef GAME_H
#define GAME_H

#include <engine/ui.h>
#include <engine/geometry.h>
#include <engine/terminal.h>
#include <stdarg.h>
#include <stdbool.h>

typedef struct {
	Terminal t;
	Rect screen;
	Size size; //game size, fixed size used to transform elements when screen resolution changes.
	int status;
} Game;


void init_colors(void);
void init_ncurses(void);
void refresh_game_size(Game* game);
Game* init_game(int width, int height);
void game_render(Game* game, int ch);
void game_loop(Game* game);
void game_end(Game* game);
bool can_game_render(Game* game);
void game_print(const char* txt, Game* game, ...);

#endif
