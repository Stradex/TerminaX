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

bool _socket_created=false;
bool _is_server=false;

int _fd_count;
int _fd_size;
int _local_socket;
struct pollfd *_pfds;

int _packets_received = 0;


int get_listener_socket_client(const char* hostname, int port) {
  int sockfd, numbytes;
  struct addrinfo hints, *serverinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

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
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return -1;
  }
  inet_ntop(p->ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);
  printf("client: connected to %s\n", s);
  freeaddrinfo(serverinfo);

  return sockfd;
}


int get_listener_socket(int port) {
  int listener;     // Listening socket descriptor
  int yes=1;        // For setsockopt() SO_REUSEADDR, below
  int rv;
  
  struct addrinfo hints, *ai, *p;
  
  //Get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
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
  
  freeaddrinfo(ai); // All done with this

  if (p == NULL) {
    return -1;
  }
  // Listen 
  if (listen(listener, 10) == -1) {
    return -1;
  }
  
  return listener;
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
  (*fd_count)--;
}


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

int net_send_packet(const char* data) {
  for (int i=0; i < _fd_count; i++) {
    if (_is_server && _pfds[i].fd == _local_socket) {
      continue;
    }
    if (send(_pfds[i].fd, data, strlen(data), 0) == -1) {
      perror("send");
    }
  } 

  return 0;
}


int net_host(int port) {
  printf("hosting server at port %d...\n", port);
  _fd_count = 0;
  _fd_size = 2;
  _pfds = malloc((sizeof *_pfds)*_fd_size);

  //set up and get a listener socket
  _local_socket = get_listener_socket(port);

  if (_local_socket == -1) {
    fprintf(stderr, "error getting listener socket\n");
    exit(1);
  }
  
  //Add listener to set
  _pfds[0].fd = _local_socket;
  _pfds[0].events = POLLIN;  //Report ready-to-read on incoming connection
  _fd_count = 1; //for the listener

  //We call poll in the engine_frame/tic
  _socket_created = true;
  _is_server = true;
  printf("server started\n");
}

int net_connect(const char* hostname, int port) {
  printf("connecting %s:%d...\n", hostname, port);

  int nbytes;

  _local_socket = get_listener_socket_client(hostname, port);
  if (_local_socket == -1) {
    fprintf(stderr, "client: error getting socket\n");
    exit(1);
  }

  _pfds = malloc(2*sizeof(*_pfds));
  _pfds[0].fd = _local_socket; //SOCKET
  _pfds[0].events = POLLIN; // report ready-to-read events
  _fd_count = 1;

  _socket_created = true;
  _is_server = false;
  printf("client connected\n");
}

void net_server_frame(NetworkSettings* net_config, Renderer* r) {

  engine_print("server frame: clients: %d\n", r, _fd_count-1);
  int newfd;                          // Newly accept()ed socket descriptor
  struct sockaddr_storage remoteaddr; //Client address
  socklen_t addrlen;
  char buf[256];                      // Buffer for client data
  char remoteIP[INET6_ADDRSTRLEN];

  int poll_count = poll(_pfds, _fd_count, NET_FRAME_MS);
  if (poll_count == -1) {
    perror("poll");
    exit(1);
  }

  //Run through the existing connections looking for data to read.
  for (int i=0; i < _fd_count; i++) {
    //Check if someone's ready to read
    if (_pfds[i].revents & POLLIN) {  //We got one!!
      if (_pfds[i].fd == _local_socket) {
        //If _local_socket is ready to read, handle new connection

        addrlen = sizeof remoteaddr;
        newfd = accept(_local_socket, (struct sockaddr *)&remoteaddr, &addrlen);
        if (newfd == -1) {
          perror("accept");
        } else {
          add_to_pdfs(&_pfds, newfd, &_fd_count, &_fd_size);
          printf("pollserver: new connection from %s on socket %d\n", inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd);
          if (net_config->client_connect) {
            net_config->client_connect();
          }
        }
      } else {
        //If not the _local_socket, we're just a regular client.
        memset(buf, 0, sizeof(buf));
        int nbytes = recv(_pfds[i].fd, buf, sizeof buf, 0);
        int sender_fd = _pfds[i].fd;
        
        if (nbytes <= 0) {
          //Got error or connection closed by client
          if (nbytes == 0) {
            //connection closed
            printf("pollserver: socket %d hung up\n", sender_fd);
          } else {
            perror("recv");
          }

          close(_pfds[i].fd); //bye
          del_from_pdfs(&_pfds, i, &_fd_count);
          if (net_config->client_disconnect) {
            net_config->client_disconnect();
          }
        } else {
          //We got some data from a client
          printf("client: %s\n", buf);

          for (int j=0; j < _fd_count; j++) {
            // Send to everyone
            int dest_fd = _pfds[j].fd;
            if (dest_fd == _local_socket || dest_fd == sender_fd) {
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
}

void net_client_frame(NetworkSettings* net_config, Renderer* r) {
  engine_print("client frame: %d", r, _packets_received);
  char buf[MAXDATASIZE];
  int nbytes;

  int poll_count = poll(_pfds, 1, NET_FRAME_MS);
  if (poll_count == -1) {
    perror("poll");
    exit(1);
  }
  if (_pfds[0].revents & POLLIN) {
    memset(buf, 0, sizeof(buf));
    nbytes = recv(_pfds[0].fd, buf, sizeof buf, 0);
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
