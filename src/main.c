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

void game_frame(void) {
  
  Asset* tmp_asset = get_asset("anim_test"); //LOAD FROM JSON FILE
  if (!tmp_asset) return;
  UISpriteAnim* anim = (UISpriteAnim*)tmp_asset->ui_figure;
  anim->pos.x = 15;
  anim->pos.y = 2;
	UIText text = create_uitext(create_rect(34, 6, 25, 1), "PODEMOS APRENDER");
	UIText textb = create_uitext(create_rect(34, 7, 25, 1), "PRUEBA");
	text.align = ALIGN_CENTER | ALIGN_MIDDLE;
	textb.align = ALIGN_CENTER | ALIGN_MIDDLE;
	draw_element(&text, my_game->renderer);
	draw_element(&textb, my_game->renderer);
  draw_element(anim, my_game->renderer);
}

int main (int argc, char* argv[]) {
	my_game = create_game(60, 15, game_frame, argc, argv);
	game_start(my_game);
	return 0;
}
