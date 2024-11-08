#ifndef PACKET_H
#define PACKET_H

#include <stdbool.h>

typedef enum {
    PACKET_INT=1,
    PACKET_BOOL=2,
    PACKET_CHAR=3,
    PACKET_STRING=5,
    PACKET_LIST=6
} PacketType;

typedef struct {
  int type;
} Packet;

typedef struct {
  int type;
  int value;
} PacketInt;

typedef struct {
  int type;
  bool value;
} PacketBool;

typedef struct {
  int type;
  char value;
} PacketChar;

typedef struct {
  int type;
  char* value;
} PacketString;

typedef struct {
  Packet* element;
  void* next;
} PacketNode;

typedef struct {
  int type;
  PacketNode* elements; //dynamic array, can even be another PacketList
} PacketList;

void* serialize_packet(Packet* msg, int *buffer_size);
Packet* deserialize_packet(void* buffer);

//helper functions
int read_packet_int(void* buff);
bool read_packet_bool(void* buff);
char read_packet_char(void* buff);
const char* read_packet_string(void* buff);

PacketInt* create_packet_int(int value);
PacketBool* create_packet_bool(bool value);
PacketChar* create_packet_char(char value);
PacketString* create_packet_string(const char* value);
PacketList* create_packet_list();
void add_packet_to_list(PacketList* list, Packet* p);

void* serialize_int(int value, int* buff_size);
void* serialize_char(char value, int* buff_size);
void* serialize_bool(bool value, int* buff_size);
void* serialize_string(const char* value, int* buff_size);

const char* stringify_packet(Packet* p);

#endif
