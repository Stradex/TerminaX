#include <game/game.h>
#include <engine/renderer.h>
#include <engine/engine.h>
#include <engine/networking.h>

Game* create_game(int width, int height, void (*game_logic)(int), int argc, char* argv[]) {
	Game *g = calloc(1, sizeof(Game));
	g->renderer = init_renderer(width, height);
	g->game_logic = game_logic;
  g->net_config = create_from_params(argc, argv);

	return g;
}


void game_start(Game* game) {
  init_net(game->net_config);
  init_engine();
	renderer_loop(game->renderer, game->net_config, game->game_logic);
	renderer_end(game->renderer);
}
