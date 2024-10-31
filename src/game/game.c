#include <game/game.h>
#include <engine/renderer.h>
#include <engine/engine.h>

Game* create_game(int width, int height, void (*game_logic)()) {
	Game *g = calloc(1, sizeof(Game));
	g->renderer = init_renderer(width, height);
	g->game_logic = game_logic;
	return g;
}


void game_start(Game* game) {
  init_engine();
	renderer_loop(game->renderer, game->game_logic);
	renderer_end(game->renderer);
}
