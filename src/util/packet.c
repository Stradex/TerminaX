#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/packet.h>

int read_packet_int(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_INT) {
    fprintf(stderr, "invalid packet type at read_packet_int\n");
    exit(1);
  }
  return ((PacketInt*)p)->value;
}

int8_t read_packet_short(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_SHORT) {
    fprintf(stderr, "invalid packet type at read_packet_short\n");
    exit(1);
  }
  return ((PacketShort*)p)->value;
}

uint8_t read_packet_ushort(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_USHORT) {
    fprintf(stderr, "invalid packet type at read_packet_ushort\n");
    exit(1);
  }
  return ((PacketUShort*)p)->value;
}

char read_packet_char(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_CHAR) {
    fprintf(stderr, "invalid packet type at read_packet_char\n");
    exit(1);
  }
  return ((PacketChar*)p)->value;
}

bool read_packet_bool(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_BOOL) {
    fprintf(stderr, "invalid packet type at read_packet_bool\n");
    exit(1);
  }
  return ((PacketBool*)p)->value;
}

const char* read_packet_string(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_STRING) {
    fprintf(stderr, "invalid packet type at read_packet_string\n");
    exit(1);
  }
  return (const char*)((PacketBool*)p)->value;
}

/*
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
  int len;
  char* value;
} PacketString;

typedef struct {
  int type;
  int count; //count of elements
  int types*; //types of each elements
  Packet* elements; //dynamic array
} PacketList;
*/

void* serialize_packet_ushort(PacketUShort* p, uint8_t *buffer_size) {
  *buffer_size = sizeof(uint8_t)*2;
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy((char *)buffer + offset, &p->value, sizeof(uint8_t));

  return buffer;
}

void* serialize_packet_short(PacketShort* p, uint8_t *buffer_size) {
  *buffer_size = sizeof(uint8_t) + sizeof(int8_t);
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy((char *)buffer + offset, &p->value, sizeof(int8_t));

  return buffer;
}


void* serialize_packet_int(PacketInt* p, uint8_t *buffer_size) {
  *buffer_size = sizeof(uint8_t) + sizeof(int);
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy((char *)buffer + offset, &p->value, sizeof(int));

  return buffer;
}

void* serialize_packet_bool(PacketBool* p, uint8_t *buffer_size) {
  *buffer_size = sizeof(uint8_t)+sizeof(bool);
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy((char *)buffer + offset, &p->value, sizeof(bool));
  return buffer;
}

void* serialize_packet_char(PacketChar* p, uint8_t *buffer_size) {
  *buffer_size = sizeof(uint8_t)+sizeof(char);
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy((char *)buffer + offset, &p->value, sizeof(char));
  return buffer;
}

void* serialize_packet_string(PacketString* p, uint8_t *buffer_size) {
  uint8_t len = (uint8_t)(strlen(p->value)) + 1;
  *buffer_size = sizeof(uint8_t)*2 + len;
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  memcpy((char *)buffer + offset, &len, sizeof(uint8_t));
  if (len > 0) {
    offset += sizeof(uint8_t);
    memcpy((char *)buffer + offset, p->value, len);
  }
  return buffer;
}

void* serialize_packet_list(PacketList* p, uint8_t *buffer_size) {
  *buffer_size = sizeof(uint8_t)*2;
  void *buffer = malloc(sizeof(uint8_t)*2+64);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  uint8_t count = 0;
  memcpy((char *)buffer, &p->type, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  PacketNode* pn;
  for (count=0, pn=p->elements; pn != NULL; pn = (PacketNode*)pn->next, count++);
  memcpy((char *)buffer + offset, &count, sizeof(uint8_t));
  offset += sizeof(uint8_t);
  for(pn=p->elements; pn != NULL; pn = (PacketNode*)pn->next) {

    uint8_t packet_buffer_size=0;
    void* packet_buffer = serialize_packet(pn->element, &packet_buffer_size);
    memcpy((char *)buffer + offset, &packet_buffer_size, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy((char *)buffer + offset, packet_buffer, packet_buffer_size);
    offset+= packet_buffer_size;
    *buffer_size += sizeof(uint8_t) + packet_buffer_size;
  }

  return buffer;
}

PacketInt* clone_packet_int(PacketInt* p) {
  PacketInt* pi = malloc(sizeof(PacketInt));
  pi->type = p->type;
  pi->value = p->value;
  return pi;
}

PacketChar* clone_packet_char(PacketChar* p) {
  PacketChar* pc = malloc(sizeof(PacketChar));
  pc->type = p->type;
  pc->value = p->value;
  return pc;
}
PacketBool* clone_packet_bool(PacketBool* p) {
  PacketBool* pb = malloc(sizeof(PacketBool));
  pb->type = p->type;
  pb->value = p->value;
  return pb;
}
PacketString* clone_packet_string(PacketString* p) {
  PacketString* ps = malloc(sizeof(PacketString));
  ps->type = p->type;
  ps->value = malloc((strlen(p->value)+1)*sizeof(char));
  strcpy(ps->value, p->value);
  return ps;
}
Packet* clone_packet(Packet* p) {
  switch (p->type) {
    case PACKET_INT:
      return (Packet*)clone_packet_int((PacketInt*)p);
    break;
    case PACKET_BOOL:
      return (Packet*)clone_packet_bool((PacketBool*)p);
    break;
    case PACKET_CHAR:
      return (Packet*)clone_packet_char((PacketChar*)p);
    break;
    case PACKET_STRING:
      return (Packet*)clone_packet_string((PacketString*)p);
    break;
  }
  return p;
}

Packet* deserialize_packet_list(void* buffer) {
  PacketList *p = (PacketList*)buffer;
  int offset = sizeof(uint8_t);
  uint8_t count=0;
  memcpy(&count, (char *)buffer + offset ,sizeof(uint8_t));
  PacketNode* first_node = NULL;
  PacketNode* prev_node = NULL;

  if (count > 0) {
    offset+=sizeof(uint8_t);
    for (int i =0; i < count; i++) {
      PacketNode* pn = malloc(sizeof(PacketNode)); 
      if (prev_node) {
        prev_node->next = pn;
      }
      uint8_t buffer_element_size = 0;
      memcpy(&buffer_element_size, (char *)buffer + offset ,sizeof(uint8_t));
      offset += sizeof(uint8_t);
      pn->element = clone_packet((Packet*)deserialize_packet((char *)buffer+offset));
      pn->next = NULL;
      offset += buffer_element_size;
      if (i==0) {
        first_node = pn;
      }

      prev_node = pn;
    }
    p->elements = first_node;
  } else {
    p->elements = NULL;
  }
  return (Packet*)p;
}

Packet* deserialize_packet_string(void* buffer) {
  PacketString *p = malloc(sizeof(PacketString));
  p->type = PACKET_STRING;
  uint8_t len=0;
  int offset =  sizeof(uint8_t); 
  memcpy(&len, buffer + offset , sizeof(uint8_t));
  offset+=sizeof(uint8_t);
  if (len > 0) {
        char* string_value = malloc(sizeof(char)*len);
        memcpy(string_value, (char*)buffer+offset, len);
        p->value = string_value;
  } else {
        p->value = NULL;
  }
  return (Packet*)p;
}

void* serialize_packet(Packet* p, uint8_t *buffer_size) {
  switch(p->type) {
    case PACKET_SHORT:
      return serialize_packet_short((PacketShort*)p, buffer_size);
    break;
    case PACKET_USHORT:
      return serialize_packet_short((PacketUShort*)p, buffer_size);
    break;
    case PACKET_INT:
      return serialize_packet_int((PacketInt*)p, buffer_size);
    break;
    case PACKET_CHAR:
      return serialize_packet_char((PacketChar*)p, buffer_size);
    break;
    case PACKET_BOOL:
      return serialize_packet_bool((PacketBool*)p, buffer_size);
    break;
    case PACKET_STRING:
      return serialize_packet_string((PacketString*)p, buffer_size);
    break;
    case PACKET_LIST:
      return serialize_packet_list((PacketList*)p, buffer_size);
    break;
  }
  return NULL;
}

Packet* deserialize_packet(void* buffer) {
  Packet* p = (Packet*) buffer;
  switch(p->type) {
    case PACKET_SHORT:
      return (Packet*)create_packet_short( *((int8_t*)(buffer + sizeof(uint8_t))) );
    break;
    case PACKET_USHORT:
      return (Packet*)create_packet_ushort( *((uint8_t*)(buffer + sizeof(uint8_t))) );
    break;

    case PACKET_INT:
      return (Packet*)create_packet_int( *((int*)(buffer + sizeof(uint8_t))) );
    break;
    case PACKET_CHAR:
      return (Packet*)create_packet_char( *((char*)(buffer + sizeof(uint8_t))) );
    break;
    case PACKET_BOOL:
      return (Packet*)create_packet_bool( *((bool*)(buffer + sizeof(uint8_t))) );
    break;
    case PACKET_STRING:
      return deserialize_packet_string(buffer);
    break;
    case PACKET_LIST:
      return deserialize_packet_list(buffer);
    break;
  }
  return NULL;

}


// SHORTHAND FUNCTIONS
void* serialize_int(int value, uint8_t* buff_size) {
  PacketInt p;
  p.type = PACKET_INT;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_short(int8_t value, uint8_t* buff_size) {
  PacketShort p;
  p.type = PACKET_SHORT;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_ushort(uint8_t value, uint8_t* buff_size) {
  PacketUShort p;
  p.type = PACKET_USHORT;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_char(char value, uint8_t* buff_size) {
  PacketChar p;
  p.type = PACKET_CHAR;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_bool(bool value, uint8_t* buff_size) {
  PacketChar p;
  p.type = PACKET_BOOL;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_string(const char* value, uint8_t* buff_size) {
  PacketString p;
  p.type = PACKET_BOOL;
  p.value = (char* )value;

  return serialize_packet((Packet*)&p, buff_size);
}


PacketInt* create_packet_int(int value) {
  PacketInt* p = malloc(sizeof(PacketInt));
  p->type = PACKET_INT;
  p->value = value;
  return p;
}

PacketShort* create_packet_short(int8_t value) {
  PacketShort* p = malloc(sizeof(PacketShort));
  p->type = PACKET_SHORT;
  p->value = value;
  return p;
}

PacketUShort* create_packet_ushort(uint8_t value) {
  PacketUShort* p = malloc(sizeof(PacketUShort));
  p->type = PACKET_USHORT;
  p->value = value;
  return p;
}

PacketBool* create_packet_bool(bool value) {
  PacketBool* p = malloc(sizeof(PacketBool));
  p->type = PACKET_BOOL;
  p->value = value;
  return p;
}

PacketChar* create_packet_char(char value) {
  PacketChar* p = malloc(sizeof(PacketChar));
  p->type = PACKET_CHAR;
  p->value = value;
  return p;
}

PacketString* create_packet_string(const char* value) {
  PacketString* p = malloc(sizeof(PacketString));
  p->type = PACKET_STRING;
  p->value = (char*)value;
  return p;
}

PacketList* create_packet_list() {
  PacketList* p = malloc(sizeof(PacketList));
  p->type = PACKET_LIST;
  p->elements = NULL;
  return p;
}

void add_packet_to_list(PacketList* list, Packet* p) {
  //lal
  PacketNode* pn = malloc(sizeof(PacketNode));
  pn->element = p;
  pn->next = NULL;
  if (!list->elements) {
    list->elements = pn;
    return;
  }
  PacketNode* t_pn;
  for (t_pn = list->elements; t_pn->next != NULL; t_pn = t_pn->next);
  t_pn->next = pn;
}

const char* stringify_packet_ushort(PacketUShort* p) {
  char* buf = malloc(sizeof(char)*16);
  sprintf(buf, "SHORT: %u", p->value);
  return (const char*)buf;
}

const char* stringify_packet_short(PacketShort* p) {
  char* buf = malloc(sizeof(char)*16);
  sprintf(buf, "SHORT: %d", p->value);
  return (const char*)buf;
}

const char* stringify_packet_int(PacketInt* p) {
  char* buf = malloc(sizeof(char)*32);
  sprintf(buf, "INT: %d", p->value);
  return (const char*)buf;
}
const char* stringify_packet_char(PacketChar* p) {
  char* buf = malloc(sizeof(char)*16);
  sprintf(buf, "CHAR: %c", p->value);
  return (const char*)buf;
}
const char* stringify_packet_bool(PacketBool* p) {
  char* buf = malloc(sizeof(char)*16);
  sprintf(buf, "BOOL: %s", p->value ? "TRUE" : "FALSE");
  return (const char*)buf;
}

const char* stringify_packet_string(PacketString* p) {
  char* buf = malloc(sizeof(char)*(strlen(p->value)+16));
  sprintf(buf, "STRING: %s", p->value);
  return (const char*)buf;
}
const char* stringify_packet_list(PacketList* p) {
  char *buf = malloc(sizeof(char)*256);
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "List: ");
  PacketNode* pn;
  if (!p->elements) {
    sprintf(buf, "List empty");
  }
  
  for (pn = p->elements; pn != NULL; pn = pn->next) {
    char* packet_child_str = stringify_packet(pn->element); 
    sprintf(buf, "%s - %s", buf, packet_child_str);
    free(packet_child_str);
  }
  return (const char*)buf;
}

const char* stringify_packet(Packet* p) {
  switch(p->type) {
    case PACKET_SHORT:
      return stringify_packet_short((PacketShort*)p);
    break;
    case PACKET_USHORT:
      return stringify_packet_ushort((PacketShort*)p);
    break;

    case PACKET_INT:
      return stringify_packet_int((PacketInt*)p);
    break;
    case PACKET_BOOL:
      return stringify_packet_bool((PacketBool*)p);
    break;
    case PACKET_CHAR:
      return stringify_packet_char((PacketChar*)p);
    break;
    case PACKET_STRING:
      return stringify_packet_string((PacketString*)p);
    break;
    case PACKET_LIST:
      return stringify_packet_list((PacketList*)p);
    break;
  }
  return NULL;
}
