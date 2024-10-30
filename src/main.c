#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <engine/renderer.h>

Renderer* current_renderer;

int main (void) {
	current_renderer = init_renderer(60, 15); //fixed size: 15 cols - 30 rows
	renderer_loop(current_renderer);
	renderer_end(current_renderer);
	return 0;
}
