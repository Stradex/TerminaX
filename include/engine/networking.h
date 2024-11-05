#ifndef NETWORKING_H
#define NETWORKING_H

#include <engine/renderer.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXDATASIZE 64
#define DEFAULT_PORT 10666
#define NET_FRAME_MS 25

typedef struct {
  int userid;
  char* username;
  bool is_client;
} NetUserData;

typedef struct {
  bool is_server;
  bool is_net_game;
  int port;
  char* server_hostname;
	void (*client_connect)();
	void (*client_disconnect)();
	void (*server_disconnect)();
	void (*packet_recv)(const char* buf);
} NetworkSettings;

void* get_in_addr(struct sockaddr *sa);

const char* port_as_str(const int port);

NetworkSettings* create_from_params(int argc, char* argv[]);

bool is_net_server();
bool is_net_client();
bool is_net_game();
void net_frame( NetworkSettings* net_config, Renderer* r);
int net_send_packet(const char* data);
void init_net(NetworkSettings* net_config);

#endif
