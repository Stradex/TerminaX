#include <engine/networking.h>
#include <engine/renderer.h>
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

/************************************************
* NET STRUCTURE:
*   TCP -> Handling connections and important game events
*   UPD -> frame packets / non vital packets (ignore packets from
*         non TCP connected client listed at _pfds dynamic array)
*************************************************/

bool _socket_created=false;
bool _is_server=false;

NetConnection* _connections = NULL;

int _packets_received = 0;

bool sockaddr_equal(const struct sockaddr *sa1, const struct sockaddr *sa2) {
    // Check if the address families match
    if (sa1->sa_family != sa2->sa_family) {
        return false; // Different address families, not equal
    }

    // Compare IPv4 addresses
    if (sa1->sa_family == AF_INET) {
        struct sockaddr_in *addr_in1 = (struct sockaddr_in *)sa1;
        struct sockaddr_in *addr_in2 = (struct sockaddr_in *)sa2;

        // Compare IP addresses and ports
        return (addr_in1->sin_port == addr_in2->sin_port) &&
               (addr_in1->sin_addr.s_addr == addr_in2->sin_addr.s_addr);
    }

    // Compare IPv6 addresses
    if (sa1->sa_family == AF_INET6) {
        struct sockaddr_in6 *addr_in6_1 = (struct sockaddr_in6 *)sa1;
        struct sockaddr_in6 *addr_in6_2 = (struct sockaddr_in6 *)sa2;

        // Compare IP addresses and ports
        return (addr_in6_1->sin6_port == addr_in6_2->sin6_port) &&
               (memcmp(&addr_in6_1->sin6_addr, &addr_in6_2->sin6_addr, sizeof(struct in6_addr)) == 0);
    }

    return false; // Unsupported address family
}


void copy_sockaddr(struct sockaddr *dest, const struct sockaddr *src) {
    size_t size;
    if (src->sa_family == AF_INET) {
        size = sizeof(struct sockaddr_in); // IPv4
    } else if (src->sa_family == AF_INET6) {
        size = sizeof(struct sockaddr_in6); // IPv6
    } else {
        fprintf(stderr, "Unsupported address family\n");
        return;
    }

    // Copy the structure using memcpy
    memcpy(dest, src, size);
}

NetConnection* get_local_connection() {
  NetConnection* p;
  int i=0;
  for (i=0, p = _connections; p != NULL; i++, p = (NetConnection*) p->next) {
    if (p->is_local) {
      return p;
    }
  }
  return NULL;
}


SocketConnection *get_listener_socket_client(const char* hostname, int port, int protocol) {
  SocketConnection* socket_info = calloc(1, sizeof(SocketConnection));
  int sockfd, numbytes;
  struct addrinfo hints, *serverinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = (protocol == NET_PROTO_TCP) ? SOCK_STREAM : SOCK_DGRAM;

  if ((rv = getaddrinfo(hostname, port_as_str(port), &hints, &serverinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }

  // loop through all the results and connect to the first we can
  for (p=serverinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1 ) {
      perror("client: socket");
      continue;
    }
    if (protocol == NET_PROTO_TCP && connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return NULL;
  }

  copy_sockaddr(&(socket_info->addr), (struct sockaddr*)p->ai_addr);
  socket_info->addr_len = p->ai_addrlen;
  socket_info->fd = sockfd;

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);
  printf("client: connected to %s\n", s);
  freeaddrinfo(serverinfo);

  return socket_info;
}


SocketConnection *get_listener_socket(int port, int protocol) {
  SocketConnection* socket_info = calloc(1, sizeof(SocketConnection));
  int listener;     // Listening socket descriptor
  int yes=1;        // For setsockopt() SO_REUSEADDR, below
  int rv;
  
  struct addrinfo hints, *ai, *p;
  
  //Get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = (protocol == NET_PROTO_TCP) ? SOCK_STREAM : SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, port_as_str(port), &hints, &ai)) != 0) {
    fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
    exit(1);
  }

  for (p = ai; p != NULL; p = p->ai_next) {
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }

    // Lose the pesky "address already in use" error message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) { 
      close(listener);
      continue;
    }

    break;
  }
  
  copy_sockaddr(&(socket_info->addr), (struct sockaddr*)p->ai_addr);
  socket_info->addr_len = p->ai_addrlen;
  socket_info->fd = listener;

  freeaddrinfo(ai); // All done with this

  if (p == NULL) {
    return NULL;
  }
  // Listen 
  if (protocol == NET_PROTO_TCP && listen(listener, 10) == -1) {
    return NULL;
  }
  
  return socket_info;
}

// Add a new file descriptor to the set

void add_to_pdfs(struct pollfd* pdfs[], int newfd, int *fd_count, int* fd_size) {
  // If we don't have room, add more space in the pdfs array
  if (*fd_count == *fd_size) {
    *fd_size *= 2;
    *pdfs = realloc(*pdfs, sizeof(**pdfs)*(*fd_size)); 
  }
  (*pdfs)[*fd_count].fd = newfd;
  (*pdfs)[*fd_count].events = POLLIN; // Check ready-to-read

  (*fd_count)++;
}

// Remove an index from the set 
void del_from_pdfs(struct pollfd* pdfs[], int i, int *fd_count) {
  // Copy the one from the end over this one
  pdfs[i] = pdfs[*fd_count-1];

  // TODO: remove from _udp_pfds
  (*fd_count)--;
}


// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in* )sa)->sin_addr);
  }
  return &(((struct sockaddr_in6* )sa)->sin6_addr);
}

int get_port_from_sockaddr(struct sockaddr* sa) {
  if (sa->sa_family == AF_INET) {
      struct sockaddr_in *addr_in = (struct sockaddr_in *)sa;
      return addr_in->sin_port;
  }
  if (sa->sa_family == AF_INET6) {
      struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)sa;
      return addr_in6->sin6_port;
  }
  return -1;
}

char* get_ip_from_sockaddr(struct sockaddr* sa) {
  char s[INET6_ADDRSTRLEN];
  char* ret;

  ret = calloc(INET6_ADDRSTRLEN+1, sizeof(char));
  sprintf(ret, "%s", inet_ntop(sa->sa_family, get_in_addr(sa), s, sizeof s));
  return ret;
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

int net_send_packet(const char* data, int protocol) {
  NetConnection* local_net = get_local_connection();
  NetConnection *p = NULL;
  for (p=_connections; p != NULL; p = (NetConnection*)p->next) {
    if (_is_server && p->is_local) {
      continue;
    }
    switch (protocol) {
      case NET_PROTO_TCP:
        if (send(p->tcp_pfd.fd, data, strlen(data), 0) == -1) {
          perror("send");
        }
      break;
      case NET_PROTO_UDP:
       if (sendto(local_net->udp_pfd.fd, data, strlen(data), 0, &(p->udp_addr), p->udp_addr_len) == -1) {
          perror("sendto");
       }
      break;
    }
  } 

  return 0;
}


int net_host(int port) {
  int listener_tcp, listener_udp;
  printf("hosting server at port %d...\n", port);
  SocketConnection* tcp_info; 
  SocketConnection* udp_info; 
  //set up and get a listener socket
  tcp_info = get_listener_socket(port, NET_PROTO_TCP);
  udp_info = get_listener_socket(port, NET_PROTO_UDP);
  if (!tcp_info) {
    fprintf(stderr, "error getting tcp socket\n");
    exit(1);
  }
  if (!udp_info) {
    fprintf(stderr, "error getting udp socket\n");
    exit(1);
  }
  
  _connections = calloc(1, sizeof(NetConnection));
  _connections->is_local = true;
  _connections->next = NULL;
  _connections->tcp_pfd.fd = tcp_info->fd;
  _connections->tcp_pfd.events = POLLIN;
  copy_sockaddr( &(_connections->tcp_addr), &(tcp_info->addr) );
  _connections->tcp_addr_len = tcp_info->addr_len;
  _connections->udp_pfd.fd = udp_info->fd;
  copy_sockaddr( &(_connections->udp_addr), &(udp_info->addr) );
  _connections->udp_addr_len = udp_info->addr_len;
  _connections->udp_pfd.events = POLLIN;

  //We call poll in the engine_frame/tic
  _socket_created = true;
  _is_server = true;
  printf("server started\n");
}

int net_connect(const char* hostname, int port) {
  printf("connecting %s:%d...\n", hostname, port);

  int nbytes;

  SocketConnection* tcp_info; 
  SocketConnection* udp_info; 

  tcp_info = get_listener_socket_client(hostname, port, NET_PROTO_TCP);
  udp_info = get_listener_socket_client(hostname, port, NET_PROTO_UDP);
  if (!tcp_info) {
    fprintf(stderr, "client: error getting socket tcp\n");
    exit(1);
  }
  if (!udp_info) {
    fprintf(stderr, "client: error getting socket udp\n");
    exit(1);
  }

  _connections = calloc(1, sizeof(NetConnection));
  _connections->is_local = true;
  _connections->next = NULL;
  _connections->tcp_pfd.fd = tcp_info->fd;
  _connections->tcp_pfd.events = POLLIN;
  copy_sockaddr( &(_connections->tcp_addr), &(tcp_info->addr) );
  _connections->tcp_addr_len = tcp_info->addr_len;
  _connections->udp_pfd.fd = udp_info->fd;
  _connections->udp_pfd.events = POLLIN;
  copy_sockaddr( &(_connections->udp_addr), &(udp_info->addr) );
  _connections->udp_addr_len = udp_info->addr_len;

  _socket_created = true;
  _is_server = false;
  printf("client connected\n");
}

int get_connections_count() {
  NetConnection* p;
  int i=0;
  for (p = _connections; p != NULL; p = (NetConnection*) p->next) {
    if (p->is_local) {
      continue;
    }
    i++;
  }
  return i;

}

void add_net_connection(NetConnection* n) {
  NetConnection* p;
  for (p = _connections; p->next != NULL; p = (NetConnection *)p->next);

  if (p==NULL) {
    fprintf(stderr, "failed to add new connection\n");
    exit(1);
  }

  p->next = n;
}


bool delete_net_connection_by_tcp_fd(int tcp_fd) {
  NetConnection* current=NULL;
  NetConnection* previous=NULL;
  for (current = _connections; current != NULL; previous = current, current = (NetConnection*) current->next) {
    if (previous && current->tcp_pfd.fd == tcp_fd) {
      previous->next = current->next;
      return true;
    }
  }
  return false;

}

NetConnection* get_conn_by_sockaddr(const struct sockaddr* addr) {
  NetConnection* p;
  int i;
  for (i=0, p = _connections; p != NULL; i++, p = (NetConnection*)p->next) {
    if (sockaddr_equal(addr, (const struct sockaddr*)&p->udp_addr)) {
      return p;
    }
    if (sockaddr_equal(addr, (const struct sockaddr*)&p->tcp_addr)) {
      return p;
    }

  }
  return NULL;
}


NetConnection* get_net_connection(int index) {
  NetConnection* p;
  int i;
  for (i=0, p = _connections; p != NULL; i++, p = (NetConnection*)p->next) {
    if (i == index) {
      return p;
    }
  }
  return NULL;
}

ListPollfd get_all_pfds(int protocol) {
  ListPollfd pfds_list;
  pfds_list.pfds = NULL;
  pfds_list.size = 0;
  NetConnection* p;
  int i=0;
  for (i=0, p = _connections; p != NULL; i++, p = (NetConnection*) p->next) {
    pfds_list.pfds=(struct pollfd*)realloc(pfds_list.pfds, (i+1)*sizeof(struct pollfd));
    if (protocol == NET_PROTO_TCP) {
      if (p->tcp_pfd.fd == NET_UNUSED_FD) {
        continue;
      }
      pfds_list.pfds[pfds_list.size] = p->tcp_pfd;
    } else {
      if (p->udp_pfd.fd == NET_UNUSED_FD) {
        continue;
      }
      pfds_list.pfds[pfds_list.size] = p->udp_pfd;
    }
    pfds_list.size++;
  }
  
  return pfds_list;
}


void net_handle_udp(NetworkSettings* net_config, Renderer* r) {
  ListPollfd pfds_list = get_all_pfds(NET_PROTO_UDP);
  NetConnection* local_net = get_local_connection();
  struct sockaddr_storage their_addr;
  socklen_t addr_len;
  int numbytes;
  char buf[MAXDATASIZE];
  char s[INET6_ADDRSTRLEN];
  if (!local_net) {
    fprintf(stderr, "server: local connection does not exists!\n");
    exit(1);
  }

  int poll_count = poll(pfds_list.pfds, pfds_list.size, NET_FRAME_MS);
  if (poll_count == -1) {
    perror("poll");
    exit(1);
  }

  for (int i=0; i < pfds_list.size; i++) {
    if (pfds_list.pfds[i].fd != local_net->udp_pfd.fd) {
      continue;
    }

    if (pfds_list.pfds[i].revents & POLLIN) {
      addr_len = sizeof their_addr;
      if ((numbytes = recvfrom(local_net->udp_pfd.fd, buf, MAXDATASIZE-1, 0, (struct sockaddr*)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
      }

      if (_is_server) {
        NetConnection* sender_conn = get_conn_by_sockaddr((const struct sockaddr*)&their_addr); 
        if (!sender_conn) {
          sender_conn = calloc(1, sizeof(NetConnection));
          sender_conn->is_local = false;
          sender_conn->next = NULL;
          sender_conn->tcp_pfd.fd = NET_UNUSED_FD;
          sender_conn->udp_pfd.fd = NET_CLIENT_UDP_FD;
          copy_sockaddr( &(sender_conn->udp_addr), (struct sockaddr *)&their_addr );
          sender_conn->udp_addr_len = addr_len;
          add_net_connection(sender_conn);

        }
      } 
      buf[numbytes] = '\0';
      if (net_config->packet_recv) {
        net_config->packet_recv(buf);
      }
    }
  }
}

void net_server_handle_tcp(NetworkSettings* net_config) {
  ListPollfd pfds_list = get_all_pfds(NET_PROTO_TCP);
  int newfd;                          // Newly accept()ed socket descriptor
  struct sockaddr_storage remoteaddr; //Client address
  socklen_t addrlen;
  char buf[256];                      // Buffer for client data
  char remoteIP[INET6_ADDRSTRLEN];
  int local_tcp_fd = (get_local_connection())->tcp_pfd.fd; 

  int poll_count = poll(pfds_list.pfds, pfds_list.size, NET_FRAME_MS);
  if (poll_count == -1) {
    perror("poll");
    exit(1);
  }

  //Run through the existing _connections looking for data to read.
  for (int i=0; i < pfds_list.size; i++) {
    //Check if someone's ready to read
    if (pfds_list.pfds[i].revents & POLLIN) {  //We got one!!
      if (pfds_list.pfds[i].fd == local_tcp_fd) {
        //If _local_socket is ready to read, handle new connection

        addrlen = sizeof remoteaddr;
        newfd = accept(local_tcp_fd, (struct sockaddr *)&remoteaddr, &addrlen);
        if (newfd == -1) {
          perror("accept");
        } else {
          NetConnection* new_conn = calloc(1, sizeof(NetConnection));
          new_conn->is_local = false;
          new_conn->next = NULL;
          new_conn->tcp_pfd.fd = newfd;
          new_conn->tcp_pfd.events = POLLIN;
          copy_sockaddr( &(new_conn->tcp_addr), (struct sockaddr *)&remoteaddr );
          new_conn->tcp_addr_len = addrlen;
          new_conn->udp_pfd.fd = NET_UNUSED_FD;

          add_net_connection(new_conn);

          printf("pollserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
          if (net_config->client_connect) {
            net_config->client_connect();
          }
        }
      } else {
        //If not the _local_socket, we're just a regular client.
        memset(buf, 0, sizeof(buf));
        int nbytes = recv(pfds_list.pfds[i].fd, buf, sizeof buf, 0);
        int sender_fd = pfds_list.pfds[i].fd;
       
        if (nbytes <= 0) {
          //Got error or connection closed by client
          if (nbytes == 0) {
            //connection closed
            printf("pollserver: socket %d hung up\n", sender_fd);
          } else {
            perror("recv");
          }

          close(pfds_list.pfds[i].fd); //bye
          if (!delete_net_connection_by_tcp_fd(pfds_list.pfds[i].fd)) {
            fprintf(stderr, "server: error trying to delete connection\n");
            exit(1);
          }

          if (net_config->client_disconnect) {
            net_config->client_disconnect();
          }
        } else {
          //We got some data from a client
          printf("client: %s\n", buf);
          _packets_received++;

          for (int j=0; j < pfds_list.size; j++) {
            // Send to everyone
            int dest_fd = pfds_list.pfds[j].fd;
            if (dest_fd == local_tcp_fd || dest_fd == sender_fd) {
              continue;
            }
            if (send(dest_fd, buf, nbytes, 0) == -1) {
              perror("send");
            }

          }

          if (net_config->packet_recv) {
            net_config->packet_recv(buf);
          }

        }
      }
    }
  }

  if (pfds_list.pfds) {
    free(pfds_list.pfds);
  }
}

void net_server_frame(NetworkSettings* net_config, Renderer* r) {
  net_server_handle_tcp(net_config);
  net_handle_udp(net_config, r);
}

void net_client_handle_tcp(NetworkSettings* net_config) {
  ListPollfd pfds_list = get_all_pfds(NET_PROTO_TCP);
  int newfd;                          // Newly accept()ed socket descriptor
  char buf[MAXDATASIZE];
  int nbytes;

  int poll_count = poll(pfds_list.pfds, pfds_list.size, NET_FRAME_MS);
  if (poll_count == -1) {
    perror("poll");
    exit(1);
  }

  if (pfds_list.pfds[0].revents & POLLIN) {
    memset(buf, 0, sizeof(buf));
    nbytes = recv(pfds_list.pfds[0].fd, buf, sizeof buf, 0);
    if (nbytes <= 0) {
      if (nbytes == 0) {
        fprintf(stderr, "client: server stopped\n");
      } else {
        fprintf(stderr, "client: recv error\n");
      }

      exit(1);
    }
    if (net_config->packet_recv) {
      net_config->packet_recv(buf);
    }

    _packets_received++;
  }


}
void net_client_frame(NetworkSettings* net_config, Renderer* r) {
  net_client_handle_tcp(net_config);
  net_handle_udp(net_config, r);
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

void net_frame(NetworkSettings* net_config, Renderer* r) {

  if (!_socket_created) {
    return;
  }
  if (_is_server) {
    net_server_frame(net_config, r);
  } else {
    net_client_frame(net_config, r);
  }
}
