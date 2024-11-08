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
#include <poll.h>

#define MAXDATASIZE         100
#define DEFAULT_PORT        10666
#define NET_FRAME_MS        20
#define NET_PROTO_UDP       0
#define NET_PROTO_TCP       1
#define NET_UNUSED_FD      -2
#define NET_CLIENT_UDP_FD  -3

typedef struct {
  bool is_local;
  struct sockaddr tcp_addr;
  socklen_t tcp_addr_len;
  struct sockaddr udp_addr;
  socklen_t udp_addr_len;
  struct pollfd udp_pfd;
  struct pollfd tcp_pfd;
  void* next;
} NetConnection;

typedef struct {
  struct sockaddr addr;
  socklen_t addr_len;
  int fd;
} SocketConnection;


typedef struct {
  struct pollfd* pfds;
  int size;
} ListPollfd;

typedef struct {
  bool is_server;
  bool is_net_game;
  int port;
  char* server_hostname;
	void (*client_connect)();
	void (*client_disconnect)();
	void (*server_disconnect)();
	void (*packet_recv)(void* buf);
} NetworkSettings;

void* get_in_addr(struct sockaddr *sa);

const char* port_as_str(const int port);

NetworkSettings* create_from_params(int argc, char* argv[]);

bool is_net_server();
bool is_net_client();
bool is_net_game();
void net_frame(NetworkSettings* net_config, Renderer* r);
int net_send_packet(void* data, int protocol, int buffer_size);
void init_net(NetworkSettings* net_config);

#endif
