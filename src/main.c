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
UISpriteAnim anim;
UISpriteAnim anim2;
UISpriteAnim anim3;

void init_anim(void) {
  UISprite s1 = create_uisprite(0, 0, "****\n @@ \n****");
  s1.brightmap = "1111\n0220\n1111";
  UISprite s2 = create_uisprite(0, 0, "****\n @@ \n****");
  s2.brightmap = "2111\n0220\n1111";
  UISprite s3 = create_uisprite(0, 0, "****\n @@ \n****");
  s3.brightmap = "2211\n0220\n1111";
  UISprite s4 = create_uisprite(0, 0, "****\n @@ \n****");
  s4.brightmap = "2221\n0220\n1111";
  UISprite s5 = create_uisprite(0, 0, "****\n @@ \n****");
  s5.brightmap = "2222\n0220\n1111";
  UISprite s6 = create_uisprite(0, 0, "****\n @@ \n****");
  s6.brightmap = "1222\n0220\n2111";
  UISprite s7 = create_uisprite(0, 0, "****\n @@ \n****");
  s7.brightmap = "1122\n0220\n2211";
  UISprite s8 = create_uisprite(0, 0, "****\n @@ \n****");
  s8.brightmap = "1112\n0220\n2221";
  UISprite s9 = create_uisprite(0, 0, "****\n @@ \n****");
  s9.brightmap = "1111\n0220\n2222";
  UISprite s10 = create_uisprite(0, 0, "****\n @@ \n****");
  s10.brightmap = "1111\n0220\n1222";
  UISprite s11 = create_uisprite(0, 0, "****\n @@ \n****");
  s11.brightmap = "1111\n0220\n1122";
  UISprite s12 = create_uisprite(0, 0, "****\n @@ \n****");
  s12.brightmap = "1111\n0220\n1112";


  UISprite a2_s1 = create_uisprite(0, 0, "#");
  a2_s1.brightmap = "1";
  UISprite a2_s2 = create_uisprite(0, 0, "##");
  a2_s2.brightmap = "11";
  UISprite a2_s3 = create_uisprite(0, 0, "###");
  a2_s3.brightmap = "111";
  UISprite a2_s4 = create_uisprite(0, 0, "####");
  a2_s4.brightmap = "1111";
  UISprite a2_s5 = create_uisprite(0, 0, "####\n#");
  a2_s5.brightmap = "1111\n1";
  UISprite a2_s6 = create_uisprite(0, 0, "####\n##");
  a2_s6.brightmap = "1111\n11";
  UISprite a2_s7 = create_uisprite(0, 0, "####\n###");
  a2_s7.brightmap = "1111\n111";
  UISprite a2_s8 = create_uisprite(0, 0, "####\n####");
  a2_s8.brightmap = "1111\n1111";

  UISprite a3_s1 = create_uisprite(0, 0, "#");
  a3_s1.brightmap = "1";
  UISprite a3_s2 = create_uisprite(0, 0, "##");
  a3_s2.brightmap = "12";
  UISprite a3_s3 = create_uisprite(0, 0, "###");
  a3_s3.brightmap = "123";
  UISprite a3_s4 = create_uisprite(0, 0, "####");
  a3_s4.brightmap = "1234";
  UISprite a3_s5 = create_uisprite(0, 0, "####\n#");
  a3_s5.brightmap = "1234\n4";
  UISprite a3_s6 = create_uisprite(0, 0, "####\n##");
  a3_s6.brightmap = "1234\n56";
  UISprite a3_s7 = create_uisprite(0, 0, "####\n###");
  a3_s7.brightmap = "1234\n561";
  UISprite a3_s8 = create_uisprite(0, 0, "####\n####");
  a3_s8.brightmap = "1234\n5612";


  anim = create_uisprite_anim(2, 2);
  add_sprite_to_anim(&anim, &s1, 1);
  add_sprite_to_anim(&anim, &s2, 1);
  add_sprite_to_anim(&anim, &s3, 1);
  add_sprite_to_anim(&anim, &s4, 1);
  add_sprite_to_anim(&anim, &s5, 1);
  add_sprite_to_anim(&anim, &s6, 1);
  add_sprite_to_anim(&anim, &s7, 1);
  add_sprite_to_anim(&anim, &s8, 1);
  add_sprite_to_anim(&anim, &s9, 1);
  add_sprite_to_anim(&anim, &s10, 1);
  add_sprite_to_anim(&anim, &s11, 1);
  add_sprite_to_anim(&anim, &s12, 1);

  anim2 = create_uisprite_anim(11, 2);
  add_sprite_to_anim(&anim2, &a2_s1, 1);
  add_sprite_to_anim(&anim2, &a2_s2, 1);
  add_sprite_to_anim(&anim2, &a2_s3, 1);
  add_sprite_to_anim(&anim2, &a2_s4, 1);
  add_sprite_to_anim(&anim2, &a2_s5, 1);
  add_sprite_to_anim(&anim2, &a2_s6, 1);
  add_sprite_to_anim(&anim2, &a2_s7, 1);

  anim3 = create_uisprite_anim(22, 2);
  add_sprite_to_anim(&anim3, &a3_s1, 1);
  add_sprite_to_anim(&anim3, &a3_s2, 1);
  add_sprite_to_anim(&anim3, &a3_s3, 1);
  add_sprite_to_anim(&anim3, &a3_s4, 1);
  add_sprite_to_anim(&anim3, &a3_s5, 1);
  add_sprite_to_anim(&anim3, &a3_s6, 1);
  add_sprite_to_anim(&anim3, &a3_s7, 1);

}


void game_frame(void) {
	UIText text = create_uitext(create_rect(0, 1, 10, 1), "ANIM 1");
	UIText text2 = create_uitext(create_rect(11, 1, 22, 1), "ANIM 2");
	UIText text3 = create_uitext(create_rect(22, 1, 33, 1), "ANIM 3");
	text.align = ALIGN_CENTER | ALIGN_MIDDLE;
	text2.align = ALIGN_CENTER | ALIGN_MIDDLE;
	text3.align = ALIGN_CENTER | ALIGN_MIDDLE;
	draw_element(&text, my_game->renderer);
	draw_element(&text2, my_game->renderer);
	draw_element(&text3, my_game->renderer);
  draw_element(&anim, my_game->renderer);
  draw_element(&anim2, my_game->renderer);
  draw_element(&anim3, my_game->renderer);
}

int main (void) {
  init_anim();
	my_game = create_game(60, 15, game_frame);
	game_start(my_game);
	return 0;
}
