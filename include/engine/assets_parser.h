#ifndef ASSETS_PARSER_H
#define ASSETS_PARSER_H

#include <util/json.h>
#include <engine/ui.h>

typedef struct {
  void* ui_figure;
  char* alias;
} Asset;

extern Asset* global_assets;
extern int global_assets_count;

UISprite load_sprite_from_file(const char* file, const char* brightmap_file);

#endif
