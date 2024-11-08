#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/packet.h>

int read_packet_int(void* data) {
  Packet* p = (Packet*)data;
  if (p->type != PACKET_INT) {
    fprintf(stderr, "invalid packet type at read_packet_int\n");
    exit(1);
  }
  return ((PacketInt*)p)->value;
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

void* serialize_packet_int(PacketInt* p, int *buffer_size) {
  *buffer_size = sizeof(int)*2;
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(int));
  offset += sizeof(int);
  memcpy((char *)buffer + offset, &p->value, sizeof(int));
  return buffer;
}

void* serialize_packet_bool(PacketBool* p, int *buffer_size) {
  *buffer_size = sizeof(int)+sizeof(bool);
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(int));
  offset += sizeof(int);
  memcpy((char *)buffer + offset, &p->value, sizeof(bool));
  return buffer;
}

void* serialize_packet_char(PacketChar* p, int *buffer_size) {
  *buffer_size = sizeof(int)+sizeof(char);
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(int));
  offset += sizeof(int);
  memcpy((char *)buffer + offset, &p->value, sizeof(char));
  return buffer;
}

void* serialize_packet_string(PacketString* p, int *buffer_size) {
  int len = strlen(p->value);
  *buffer_size = sizeof(int) + sizeof(int) + len;
  void *buffer = malloc(*buffer_size);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  memcpy((char *)buffer + offset, &p->type, sizeof(int));
  offset += sizeof(int);
  memcpy((char *)buffer + offset, &len, sizeof(int));
  if (len > 0) {
    offset += sizeof(int);
    memcpy((char *)buffer + offset, p->value, len);
  }
  return buffer;
}

void* serialize_packet_list(PacketList* p, int *buffer_size) {
  *buffer_size = sizeof(int);
  void *buffer = malloc(sizeof(int)*2+64);
  if (!buffer) {
    perror("malloc failed");
    exit(1);
  }
  int offset = 0;
  int count = 0;
  memcpy((char *)buffer, &p->type, sizeof(int));
  offset += sizeof(int);
  PacketNode* pn;
  for (count=0, pn=p->elements; pn != NULL; pn = (PacketNode*)pn->next, count++);
  memcpy((char *)buffer + sizeof(int), &count, sizeof(int));
  offset += sizeof(int);
  for(pn=p->elements; pn != NULL; pn = (PacketNode*)pn->next) {
    int packet_buffer_size=0;
    void* packet_buffer = serialize_packet(pn->element, &packet_buffer_size);
    memcpy((char *)buffer + offset, &packet_buffer_size, sizeof(int));
    offset += sizeof(int);
    memcpy((char *)buffer + offset, packet_buffer, packet_buffer_size);
    offset+= packet_buffer_size;
    *buffer_size += sizeof(int) + packet_buffer_size;
  }
  return buffer;
}

Packet* deserialize_packet_list(void* buffer) {
  PacketList *p = (PacketList*)buffer;
  int offset = sizeof(int);
  int count=0;
  memcpy(&count, (char *)buffer + offset ,sizeof(int));
  PacketNode* first_node = NULL;
  PacketNode* prev_node = NULL;

  if (count > 0) {
    offset+=sizeof(int);
    for (int i =0; i < count; i++) {
      PacketNode* pn = malloc(sizeof(PacketNode)); 
      if (prev_node) {
        prev_node->next = pn;
      }
      int buffer_element_size = 0;
      memcpy(&buffer_element_size, (char *)buffer + offset ,sizeof(int));
      offset += sizeof(int);
      Packet* dp = (Packet*)deserialize_packet((char *)buffer+offset);
      pn->element = malloc(buffer_element_size);
      memcpy(pn->element, dp, buffer_element_size);
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
  PacketString *p = (PacketString*)buffer;
  int len=0;
  int offset =  sizeof(int); 
  memcpy(&len, buffer + offset , sizeof(int));
  offset+=sizeof(int);
  if (len > 0) {
        p->value = (char *) buffer + offset;
  } else {
        p->value = NULL;
  }

  return (Packet*)p;
}

void* serialize_packet(Packet* p, int *buffer_size) {
  switch(p->type) {
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
    case PACKET_INT:
    case PACKET_CHAR:
    case PACKET_BOOL:
      return p;
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
void* serialize_int(int value, int* buff_size) {
  PacketInt p;
  p.type = PACKET_INT;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_char(char value, int* buff_size) {
  PacketChar p;
  p.type = PACKET_CHAR;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_bool(bool value, int* buff_size) {
  PacketChar p;
  p.type = PACKET_BOOL;
  p.value = value;

  return serialize_packet((Packet*)&p, buff_size);
}

void* serialize_string(const char* value, int* buff_size) {
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