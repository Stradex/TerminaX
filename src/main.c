#include <stdio.h>

#ifdef __EMSCRIPTEN__
  #include <util/webcurses.h>
#else
  #include <ncurses.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <engine/renderer.h>
#include <game/game.h>
#include <engine/ui.h>
#include <engine/draw.h>
#include <engine/geometry.h>
#include <engine/assets_parser.h>
#include <util/packet.h>

Game* my_game;

void game_frame(int ch) {

  UIText text, textb; 
  text = create_uitext(create_rect(34, 6, 25, 1), "PODEMOS APRENDER"); 
  textb = create_uitext(create_rect(34, 7, 25, 1), "PRUEBA");
	text.align = ALIGN_CENTER | ALIGN_MIDDLE;
	textb.align = ALIGN_CENTER | ALIGN_MIDDLE;
	draw_element(&text, my_game->renderer);
	draw_element(&textb, my_game->renderer);
}

#ifndef __EMSCRIPTEN__
int main (int argc, char* argv[]) {
	my_game = create_game(60, 15, game_frame, argc, argv);
#else
int main () {
	my_game = create_game(60, 15, game_frame, 0, NULL);
#endif
	game_start(my_game);
  game_end(my_game);
  //test_packets();
	return 0;
}
