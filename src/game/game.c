#include <game/game.h>
#include <engine/renderer.h>
#include <engine/engine.h>
#include <engine/networking.h>
#include <engine/assets_parser.h>

Game* create_game(int width, int height, void (*game_logic)(int), int argc, char* argv[]) {
	Game *g = calloc(1, sizeof(Game));
	g->renderer = init_renderer(width, height);
	g->game_logic = game_logic;
#ifndef __EMSCRIPTEN__
  g->net_config = create_from_params(argc, argv);
#else
  g->net_config = NULL;
#endif

	return g;
}

void game_end(Game* game) {
#ifndef __EMSCRIPTEN__
  free_all_assets();
  free(game->renderer);
  free(game);
#endif
}

void game_init(Game* game) {
#ifndef __EMSCRIPTEN__
  init_net(game->net_config);
#endif
  init_engine();
}

void game_start(Game* game) {
	renderer_loop(game->renderer, game->net_config, game->game_logic);
	renderer_end(game->renderer);
}
