#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <engine/renderer.h>
#include <game/game.h>
#include <engine/ui.h>
#include <engine/draw.h>
#include <engine/geometry.h>

Game* my_game;

void game_frame(void) {
	UIText text = create_uitext(create_rect(0, 0, my_game->renderer->size.width, my_game->renderer->size.height), "GAME LOGIC TEXT");
	text.align = ALIGN_CENTER | ALIGN_MIDDLE;
	draw_element(&text, my_game->renderer);
}

int main (void) {
	my_game = create_game(60, 15, game_frame);
	game_start(my_game);
	return 0;
}
