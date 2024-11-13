
#ifndef ASSETS_PARSER_H
#define ASSETS_PARSER_H

#include <stdbool.h>

#include <util/json.h>
#include <engine/ui.h>

typedef struct {
  void* ui_figure;
  char* alias;
} Asset;

UISprite load_sprite_from_file(const char* file, const char* brightmap_file);
Asset* get_asset(const char* alias);

void free_all_assets();

bool init_assets(void);

#endif
