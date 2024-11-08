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

#define GAME_WIDTH           70
#define GAME_HEIGHT          15
#define SNAPSHOT_TICS        3

#define SYNC_PLAYER_EVENT   0
#define SYNC_FRUIT_EVENT    1
#define SYNC_PICK_EVENT     2
#define SYNC_PICK_CONFIRM   3

Game* my_game;
UIPoint player;
UIPoint net_player;
UIPoint fruit;
int player_score=0;
int player_net_score=0;

char packet_buf[200];
bool net_game_started = false;
int snapshot_timeout = SNAPSHOT_TICS;
bool waiting_pickup_confirm = false;

void local_init_game() {
  player = create_uipoint(2, 2);
  net_player = create_uipoint(0, 0);
  player.ch = '#'; 
  player.color = UI_COLOR_RED; 

  if (is_net_server()) {
    fruit = create_uipoint((rand() % GAME_WIDTH), rand() % GAME_HEIGHT);
  } else {
    fruit = create_uipoint(0, 0);
    fruit.visible = false;
  }

  fruit.ch = '@'; 
  fruit.color = UI_COLOR_GREEN; 
}

void client_connected( void ) {
  net_game_started = true;
}

void read_fruit_snapshot(PacketNode* snapshot_data) {
  int i;
  PacketNode* pn;
  for (i=0, pn=snapshot_data; pn != NULL; pn = pn->next, i++) {
    switch (i) {
      case 0: //First packet is pos x
        fruit.pos.x = (int)((PacketUShort*)pn->element)->value;
      break;
     case 1: //second packet is pos y
        fruit.pos.y = (int)((PacketUShort*)pn->element)->value;
      break;
    }
  }
  fruit.visible = true && !waiting_pickup_confirm;
}

void read_player_snapshot(PacketNode* snapshot_data) {
  int i;
  PacketNode* pn;
  for (i=0, pn=snapshot_data; pn != NULL; pn = pn->next, i++) {
    switch (i) {
      case 0: //First packet is pos x
        net_player.pos.x = (int)((PacketUShort*)pn->element)->value;
      break;
     case 1: //second packet is pos y
        net_player.pos.y = (int)((PacketUShort*)pn->element)->value;
      break;
     case 2: //third packet is char
        net_player.ch = ((PacketChar*)pn->element)->value;
      break;
    }
  }
}

void net_pickup_confirm() {
    PacketList* data = create_packet_list();
    add_packet_to_list(data, (Packet*)create_packet_ushort((uint8_t)SYNC_PICK_CONFIRM));
    add_packet_to_list(data, (Packet*)create_packet_ushort((uint8_t)fruit.pos.x));
    add_packet_to_list(data, (Packet*)create_packet_ushort((uint8_t)fruit.pos.y));

    int buff_size = 0;
    void* buff = serialize_packet((Packet*)data, &buff_size);
    
    if (buff != NULL) {
      net_send_packet(buff, NET_PROTO_UDP, buff_size);
      free_buff(buff);
    }
}

void packet_received(void* data) {
  Packet* p = deserialize_packet(data);
  
  if (p != NULL && p->type == PACKET_LIST) {
    PacketList* player_data = (PacketList*)p;
    PacketNode* pn = player_data->elements;
    switch (((PacketUShort*)pn->element)->value) {
      case SYNC_PLAYER_EVENT:
           read_player_snapshot(pn->next);
      break;
      case SYNC_FRUIT_EVENT:
           read_fruit_snapshot(pn->next);
      break;
      case SYNC_PICK_EVENT:
          if (is_net_server()) {
            fruit.pos.x = rand() % GAME_WIDTH;
            fruit.pos.y = rand() % GAME_HEIGHT;
            net_pickup_confirm();
          }
          player_net_score++;
      break;
      case SYNC_PICK_CONFIRM:
        if (is_net_client()) {
          player_score++;
          waiting_pickup_confirm = false;
          read_fruit_snapshot(pn->next);
        }
      break;
    }
  }
}

void handle_input(int ch) {
  if (ch == 'w') {
    player.pos.y -= 1;
  } else if (ch == 's') {
    player.pos.y += 1;
  } else if (ch == 'a') {
    player.pos.x -= 1;
  } else if (ch == 'd') {
    player.pos.x += 1;
  }

}

void net_sync_fruit() {
    PacketList* fruit_data = create_packet_list();
    add_packet_to_list(fruit_data, (Packet*)create_packet_ushort((uint8_t)SYNC_FRUIT_EVENT));
    add_packet_to_list(fruit_data, (Packet*)create_packet_ushort((uint8_t)fruit.pos.x));
    add_packet_to_list(fruit_data, (Packet*)create_packet_ushort((uint8_t)fruit.pos.y));

    int buff_size = 0;
    void* buff = serialize_packet((Packet*)fruit_data, &buff_size);
    
    if (buff != NULL) {
      net_send_packet(buff, NET_PROTO_TCP, buff_size); //TPC since this is important to sync
      free_buff(buff);
    }
}

void net_sync_player() {
    PacketList* player_data = create_packet_list();
    add_packet_to_list(player_data, (Packet*)create_packet_ushort((uint8_t)SYNC_PLAYER_EVENT));
    add_packet_to_list(player_data, (Packet*)create_packet_ushort((uint8_t)player.pos.x));
    add_packet_to_list(player_data, (Packet*)create_packet_ushort((uint8_t)player.pos.y));
    add_packet_to_list(player_data, (Packet*)create_packet_char(player.ch));

    int buff_size = 0;
    void* buff = serialize_packet((Packet*)player_data, &buff_size);
    
    if (buff != NULL) {
      net_send_packet(buff, NET_PROTO_UDP, buff_size);
      free_buff(buff);
    }
}

void net_pick_fruit() {
    PacketList* player_data = create_packet_list();
    add_packet_to_list(player_data, (Packet*)create_packet_ushort((uint8_t)SYNC_PICK_EVENT));

    int buff_size = 0;
    void* buff = serialize_packet((Packet*)player_data, &buff_size);
    
    if (buff != NULL) {
      net_send_packet(buff, NET_PROTO_UDP, buff_size);
      free_buff(buff);
    }
}


void net_snapshot() {
    if (snapshot_timeout > 0) {
      snapshot_timeout--;
      return;
    }
    snapshot_timeout = SNAPSHOT_TICS;
    net_sync_player();
    if (is_net_server()) {
      net_sync_fruit(); 
    }
}

void pick_fruit() {
  if (!fruit.visible) {
    return;
  }
  if (is_net_server()) {
    fruit.pos.x = rand() % GAME_WIDTH;
    fruit.pos.y = rand() % GAME_HEIGHT;
    player_score++;
  } else {
    waiting_pickup_confirm=true;
    fruit.visible = false;
  }
  net_pick_fruit();
}

void check_pick_fruit() {
  if (waiting_pickup_confirm && is_net_client()) {
    fruit.visible = false;
    return;
  }
  if (player.pos.x == fruit.pos.x && player.pos.y == fruit.pos.y) {
      pick_fruit();
  }
}

void game_frame(int ch) {
  if (!net_game_started && !is_net_client()) {
    UIText wait_player = create_uitext(create_rect(0, 0, GAME_WIDTH, GAME_HEIGHT), "WAITING FOR PLAYER");
    wait_player.align = ALIGN_MIDDLE | ALIGN_CENTER;
    draw_element(&wait_player, my_game->renderer);
    return;
  }

  UIText header_txt  = create_uitext(create_rect(0, 0, GAME_WIDTH-1, 2), "TerminaX net game test");
  char buf[200];
  sprintf(buf, "Score: %d", player_score);
  UIText score_txt  = create_uitext(create_rect(2, 1, GAME_WIDTH-3, 2), (const char*)buf);
  sprintf(buf, "Net score: %d", player_net_score);
  UIText net_score_txt  = create_uitext(create_rect(2, 1, GAME_WIDTH-3, 2), (const char*)buf);

  score_txt.align = ALIGN_MIDDLE | ALIGN_LEFT;
  net_score_txt.align = ALIGN_MIDDLE | ALIGN_RIGHT;
  header_txt.align = ALIGN_CENTER | ALIGN_MIDDLE;
  handle_input(ch);

  check_pick_fruit(); 
 
  draw_element(&player, my_game->renderer);
  draw_element(&net_player, my_game->renderer);
  draw_element(&fruit, my_game->renderer);
  draw_element(&header_txt, my_game->renderer);
  draw_element(&score_txt, my_game->renderer);
  draw_element(&net_score_txt, my_game->renderer);

  net_snapshot();
  free_all_packets();
}

int main (int argc, char* argv[]) {
	my_game = create_game(GAME_WIDTH, GAME_HEIGHT, game_frame, argc, argv);
  my_game->net_config->client_connect = client_connected;
  my_game->net_config->packet_recv = packet_received;
  game_init(my_game);
  local_init_game();
	game_start(my_game);
  game_end(my_game);
  //test_packets();
	return 0;
}
