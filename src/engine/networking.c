#include <engine/networking.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

bool _socket_created=false;
bool _is_server=false;

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in* )sa)->sin_addr);
  }
  return &(((struct sockaddr_in6* )sa)->sin6_addr);
}

const char* port_as_str(const int port) {
  char* port_str = (char*)malloc(6*sizeof(char));
  sprintf(port_str, "%d", port);
  return (const char*)port_str;
}

bool is_net_game() {
  return _socket_created;
}


NetworkSettings* create_from_params(int argc, char* argv[]) {
  NetworkSettings* n = calloc(1, sizeof(NetworkSettings));
  n->port = DEFAULT_PORT;
  if (argc >= 2 && strcmp(argv[1], "host") == 0) {
    n->is_server = true;
    n->is_net_game = true;
    n->port = (argc >= 3) ? atoi(argv[2]) : DEFAULT_PORT;
    n->server_hostname = NULL;
  } else if (argc >= 3 && strcmp(argv[1], "join") == 0) {
    n->is_server = false;
    n->server_hostname = (char*)calloc(strlen(argv[2])+1, sizeof(char));
    strcpy(n->server_hostname, argv[2]);
    n->port = (argc >= 4) ? atoi(argv[3]) : DEFAULT_PORT;
    n->is_net_game = true;
  } else {
    n->server_hostname = NULL;
    n->is_net_game = false;
  }
  n->client_disconnect = NULL;
  n->client_connect = NULL;
  n->server_disconnect = NULL;
  n->packet_recv = NULL;
}

bool is_net_server() {
  return is_net_game() && _is_server;
}

bool is_net_client() {
  return is_net_game() && !_is_server;
}

int net_send_packet(NetUserData* receiver, const char* data) {
  printf("sending packet...\n");
  return 0;
}

void init_net(NetworkSettings* net_config) {
  printf("init networking\n");
  if (!net_config->is_net_game) {
    return;
  } 
  if (net_config->is_server) {
    net_host(net_config->port);
  } else {
    net_connect(net_config->server_hostname, net_config->port);
  }
}

int net_host(int port) {
  printf("hosting server at port %d...\n", port);
}

int net_connect(const char* hostname, int port) {
  printf("connecting %s:%d...\n", hostname, port);
}

