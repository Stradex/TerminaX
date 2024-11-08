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
#include <util/packet.h>

Game* my_game;

int client_connected_timeout = 0;
int packet_sent_timeout = 0;
int packet_received_timeout = 0;
char packet_buf[200];

void client_connected( void ) {
  client_connected_timeout = 10;
}

void packet_received(void* data) {
  Packet* p = deserialize_packet(data);
  if (p != NULL) {
    memset(packet_buf, 0, sizeof(packet_buf));
    sprintf(packet_buf, "%s", stringify_packet(p));
  }
  packet_received_timeout = 10;
}

void test_packets() {
    PacketList* l = create_packet_list();
    add_packet_to_list(l, (Packet*)create_packet_int(5));
    add_packet_to_list(l, (Packet*)create_packet_int(7));

    int buff_size = 0;
    void* buff = serialize_packet((Packet*)l, &buff_size);
    //void* buff = NULL;
 
    
    if (buff != NULL) {
      net_send_packet(buff, NET_PROTO_UDP, buff_size);
      packet_sent_timeout=10;
    }
    
    if (buff) {
      free(buff);
    }

}

void game_frame(int ch) {

  if (ch == 's') {
    test_packets();
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
    textb = create_uitext(create_rect(34, 7, 25, 1), packet_buf);
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
