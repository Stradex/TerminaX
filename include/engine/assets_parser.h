#ifndef ASSETS_PARSER_H
#define ASSETS_PARSER_H

#include <util/json.h>
#include <engine/ui.h>
#include <stdbool.h>

typedef struct {
  void* ui_figure;
  char* alias;
} Asset;

UISprite load_sprite_from_file(const char* file, const char* brightmap_file);
bool init_assets(void);
Asset* get_asset(const char* alias);

void free_all_assets();

#endif
