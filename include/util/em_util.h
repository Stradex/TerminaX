#ifdef __EMSCRIPTEN__
#ifndef EM_UTIL_H
#define EM_UTIL_H

void init_em_params();
void add_int_em_param(int arg);
void add_char_em_param(char arg);
void add_string_em_param(const char* arg);


#endif
#endif
