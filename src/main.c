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

int client_connected_timeout = 0;
int packet_sent_timeout = 0;
int packet_received_timeout = 0;
char* packet_buf;

void client_connected( void ) {
  client_connected_timeout = 10;
}

void packet_received(const char* data) {
  packet_received_timeout = 10;
  packet_buf = calloc(strlen(data)+1, sizeof(data));
  strcpy(packet_buf, data);
}

void game_frame(int ch) {
 
  if (ch == 's') {
    net_send_packet("TEST PACKET", NET_PROTO_TCP);
    packet_sent_timeout=10;
  } 

  Asset* tmp_asset = get_asset("anim_test"); //LOAD FROM JSON FILE
  if (!tmp_asset) return;
  UISpriteAnim* anim = (UISpriteAnim*)tmp_asset->ui_figure;
  anim->pos.x = 15;
  anim->pos.y = 2;
  UIText text, textb; 
  if (packet_received_timeout > 0) {
	  text = create_uitext(create_rect(34, 6, 25, 1), "PACKET RECEIVED:"); 
    textb = create_uitext(create_rect(34, 7, 25, 1), packet_buf);
    packet_received_timeout--;

  } else if (packet_sent_timeout > 0) {
	  text = create_uitext(create_rect(34, 6, 25, 1), "SENDING"); 
    textb = create_uitext(create_rect(34, 7, 25, 1), "PACKET");
    packet_sent_timeout--;

  } else  if (client_connected_timeout > 0) {
	  text = create_uitext(create_rect(34, 6, 25, 1), "CLIENT"); 
    textb = create_uitext(create_rect(34, 7, 25, 1), "CONNECTED");
    client_connected_timeout--;
  } else {
	  text = create_uitext(create_rect(34, 6, 25, 1), "PODEMOS APRENDER"); 
    textb = create_uitext(create_rect(34, 7, 25, 1), "PRUEBA");
  }
	text.align = ALIGN_CENTER | ALIGN_MIDDLE;
	textb.align = ALIGN_CENTER | ALIGN_MIDDLE;
	draw_element(&text, my_game->renderer);
	draw_element(&textb, my_game->renderer);
  draw_element(anim, my_game->renderer);
}

int main (int argc, char* argv[]) {
	my_game = create_game(60, 15, game_frame, argc, argv);
  my_game->net_config->client_connect = client_connected;
  my_game->net_config->packet_recv = packet_received;
	game_start(my_game);
	return 0;
}
