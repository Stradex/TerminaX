#ifndef PACKET_H
#define PACKET_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PACKET_INT=1,
    PACKET_SHORT=2,
    PACKET_USHORT=3,
    PACKET_BOOL=4,
    PACKET_CHAR=5,
    PACKET_STRING=6,
    PACKET_LIST=7
} PacketType;

typedef struct {
  uint8_t type;
} Packet;

typedef struct {
  uint8_t type;
  int value;
} PacketInt;

typedef struct {
  uint8_t type;
  int8_t value;
} PacketShort;

typedef struct {
  uint8_t type;
  int8_t value;
} PacketUShort;

typedef struct {
  uint8_t type;
  bool value;
} PacketBool;

typedef struct {
  uint8_t type;
  char value;
} PacketChar;

typedef struct {
  uint8_t type;
  char* value;
} PacketString;

typedef struct {
  Packet* element;
  void* next;
} PacketNode;

typedef struct {
  uint8_t type;
  PacketNode* elements; //dynamic array, can even be another PacketList
} PacketList;

void* serialize_packet(Packet* msg, uint8_t *buffer_size);
Packet* deserialize_packet(void* buffer);

//helper functions
int read_packet_int(void* buff);
int8_t read_packet_short(void* buff);
uint8_t read_packet_ushort(void* buff);
bool read_packet_bool(void* buff);
char read_packet_char(void* buff);
const char* read_packet_string(void* buff);

PacketInt* create_packet_int(int value);
PacketShort* create_packet_short(int8_t value);
PacketUShort* create_packet_ushort(uint8_t value);
PacketBool* create_packet_bool(bool value);
PacketChar* create_packet_char(char value);
PacketString* create_packet_string(const char* value);
PacketList* create_packet_list();
void add_packet_to_list(PacketList* list, Packet* p);

void* serialize_int(int value, uint8_t* buff_size);
void* serialize_short(int8_t value, uint8_t* buff_size);
void* serialize_ushort(uint8_t value, uint8_t* buff_size);
void* serialize_char(char value, uint8_t* buff_size);
void* serialize_bool(bool value, uint8_t* buff_size);
void* serialize_string(const char* value, uint8_t* buff_size);

const char* stringify_packet(Packet* p);

void free_all_packets();
void free_buff(void* buff);
void free_packet(Packet* p);

#endif
