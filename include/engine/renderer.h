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
} Renderer;


void init_colors(void);
void init_ncurses(void);
void refresh_renderer_size(Renderer* renderer);
Renderer* init_renderer(int width, int height);
void renderer_render(Renderer* renderer, int ch);
void renderer_loop(Renderer* renderer);
void renderer_end(Renderer* renderer);
bool can_render(Renderer* renderer);
void engine_print(const char* txt, Renderer* renderer, ...);

#endif
