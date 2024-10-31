#include <engine/assets_parser.h>
#include <util/file_manager.h>
#include <util/json.h>
#include <engine/ui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

Asset* global_assets = NULL;
int global_assets_count = 0;


UISprite load_sprite_from_file(const char* file, const char* brightmap_file) {
  UISprite sprite = create_uisprite(0, 0, get_file_text(file));
  if (brightmap_file) {
    sprite.brightmap = (char *)get_file_text(brightmap_file);
  }
  return sprite;
}

void add_sprite_asset(const char* name, const char* file, const char* brightmap_file) {
  Asset new_asset;
  new_asset.alias = calloc(strlen(name)+1, sizeof(char));
  strcpy(new_asset.alias, name);
  UISprite *sprite = malloc(sizeof(UISprite));
  *sprite = load_sprite_from_file(file, brightmap_file); 
  new_asset.ui_figure = (void *)sprite;
  global_assets_count++;
  global_assets = realloc(global_assets, sizeof(Asset)*global_assets_count);
  global_assets[global_assets_count-1] = new_asset;

}

Asset* get_asset(const char* alias) {
  for (int i=0; i < global_assets_count; i++) {
    if (strcmp(global_assets[i].alias, alias) == 0) {
      return &global_assets[i];
    }
  }
  return NULL;
}

bool process_json_asset(const char* asset_name, json_object_t* obj) {
  printf("Processing %s\n", asset_name);

  result(json_element) type_element_result = json_object_find(obj, "type");
  if(result_is_err(json_element)(&type_element_result)) {
    typed(json_error) error = result_unwrap_err(json_element)(&type_element_result);
    fprintf(stderr, "Error getting element \"type\": %s\n", json_error_to_string(error));
    return false;
  }

  typed(json_element) type_element = result_unwrap(json_element)(&type_element_result);

  if (strcmp(type_element.value.as_string, "sprite") == 0) {
    //adding sprite
    char* geometry_file = NULL;
    char* brightmaps_file = NULL;
    result(json_element) geometry_element_result = json_object_find(obj, "geometry");
    if(result_is_err(json_element)(&geometry_element_result)) {
      typed(json_error) error = result_unwrap_err(json_element)(&geometry_element_result);
      fprintf(stderr, "Error getting element \"geometry\": %s\n", json_error_to_string(error));
      return false;
    }

    typed(json_element) geometry_element = result_unwrap(json_element)(&geometry_element_result);

    geometry_file = (char *)geometry_element.value.as_string;

    result(json_element) color_element_result = json_object_find(obj, "color");
    if(!result_is_err(json_element)(&color_element_result)) {
      typed(json_element) color_element = result_unwrap(json_element)(&color_element_result);
      brightmaps_file = (char *)color_element.value.as_string;
    }

    add_sprite_asset(asset_name, geometry_file, brightmaps_file);
  }
}

bool init_assets(void) {
  result(json_element) element_result = json_parse(get_file_text("./assets.json"));

  // Guard if
  if(result_is_err(json_element)(&element_result)) {
    typed(json_error) error = result_unwrap_err(json_element)(&element_result);
    fprintf(stderr, "Error parsing JSON: %s\n", json_error_to_string(error));
    return false;
  }

  // Extract the data
  typed(json_element) element = result_unwrap(json_element)(&element_result);
  typed(json_object) *obj = element.value.as_object;

  for(int i = 0; i < obj->count; i++) {
    typed(json_entry) entry = *(obj->entries[i]);

    typed(json_string) key = entry.key;
    typed(json_element_type) type = entry.element.type;
    typed(json_element_value) value = entry.element.value;
    if (type == JSON_ELEMENT_TYPE_OBJECT) {
      process_json_asset((const char*)key, entry.element.value.as_object);
    }
    // Do something with `key`, `type` and `value`
  }
  json_free(&element);
  return true;
}
