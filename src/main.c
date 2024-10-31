#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <engine/renderer.h>
#include <game/game.h>
#include <engine/ui.h>
#include <engine/draw.h>
#include <engine/geometry.h>
#include <engine/assets_parser.h>

Game* my_game;
UISpriteAnim anim;
UISprite test_sprite;

void init_game(void) {
}

void game_frame(void) {
  Asset* tmp_asset = get_asset("sprite_a");
  if (!tmp_asset) return;
	UIText text = create_uitext(create_rect(0, 1, 10, 1), "ANIM 1");
	text.align = ALIGN_CENTER | ALIGN_MIDDLE;
	draw_element(&text, my_game->renderer);
  draw_element(tmp_asset->ui_figure, my_game->renderer);
}

int main (void) {
  init_game();
	my_game = create_game(60, 15, game_frame);
	game_start(my_game);
	return 0;
}
