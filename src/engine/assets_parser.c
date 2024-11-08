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

void add_anim_asset(const char* name, json_object_t* frames_obj, bool loop) {
  Asset new_asset;
  new_asset.alias = calloc(strlen(name)+1, sizeof(char));
  strcpy(new_asset.alias, name);
  UISpriteAnim *anim = malloc(sizeof(UISpriteAnim));
  *anim = create_uisprite_anim(0, 0);

  int* frames_order;
  int frames_count=0;
  Asset** frames_assets;
  frames_assets = realloc(frames_assets, sizeof(Asset*));
   
  for (int phase =0; phase < 2; phase++) {
    frames_count=0;
    for(int i = 0; i < frames_obj->count; i++) {
      int frame = 0;
      typed(json_entry) entry = *(frames_obj->entries[i]);

      typed(json_string) key = entry.key;
      typed(json_element_type) type = entry.element.type;
      typed(json_element_value) value = entry.element.value;
      if (type != JSON_ELEMENT_TYPE_OBJECT) {
        continue;
      }

      typed(json_object)* frame_data = value.as_object;

      result(json_element) frame_element_result = json_object_find(frame_data, "frame");
      if(!result_is_err(json_element)(&frame_element_result)) {
        typed(json_element) frame_element = result_unwrap(json_element)(&frame_element_result);
        frame = (int)frame_element.value.as_number.value.as_long;
      }
      Asset* sprite_asset = get_asset((const char*) key);
      if (!sprite_asset) continue;

      switch (phase) {
        case 0:
          frames_count++;
          frames_order = realloc(frames_order, frames_count*sizeof(int));
          frames_assets = realloc(frames_assets, frames_count*sizeof(Asset*));
          frames_order[frames_count-1] = frame;
          frames_assets[frames_count-1] = sprite_asset;
        break;
        case 1:
          add_sprite_to_anim(anim, (UISprite*)frames_assets[frames_count++]->ui_figure, 1);

        break;
      }
    }

    if (phase == 0) {
      //dirty trick. Let's allocate one space more for a temporal Asset* pointer
      //so I can order my stuff
      frames_assets = realloc(frames_assets, (frames_count+1)*sizeof(Asset*));
      bool sorted=false;
      while (!sorted) {
        sorted = true;
        for (int j=0; j < frames_count-1; j++) {
          if (frames_order[j] > frames_order[j+1]) {
            int tmp_order = frames_order[j];
            frames_assets[frames_count] = frames_assets[j];
            frames_assets[j] = frames_assets[j+1];
            frames_order[j] = frames_order[j+1];
            frames_order[j+1] = tmp_order;
            frames_assets[j+1] = frames_assets[frames_count];
            sorted = false;
          }
        }
      }
      //sort frames_order
    }
  }

  anim->loop = true;
  new_asset.ui_figure = (void *)anim;
  global_assets_count++;
  global_assets = realloc(global_assets, sizeof(Asset)*global_assets_count);
  global_assets[global_assets_count-1] = new_asset;
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


bool process_json_sprite(const char* asset_name, json_object_t* obj) {
  result(json_element) type_element_result = json_object_find(obj, "type");
  if(result_is_err(json_element)(&type_element_result)) {
    typed(json_error) error = result_unwrap_err(json_element)(&type_element_result);
    fprintf(stderr, "Error getting element \"type\": %s\n", json_error_to_string(error));
    return false;
  }

  typed(json_element) type_element = result_unwrap(json_element)(&type_element_result);

  if (strcmp(type_element.value.as_string, "sprite") != 0) {
    return true;
  }

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

  return true;
}

bool process_json_anim(const char* asset_name, json_object_t* obj) {
  result(json_element) type_element_result = json_object_find(obj, "type");
  if(result_is_err(json_element)(&type_element_result)) {
    typed(json_error) error = result_unwrap_err(json_element)(&type_element_result);
    fprintf(stderr, "Error getting element \"type\": %s\n", json_error_to_string(error));
    return false;
  }

  typed(json_element) type_element = result_unwrap(json_element)(&type_element_result);

  if (strcmp(type_element.value.as_string, "sprite_anim") != 0) { 
    return true;
  }
  bool anim_loop=false;
  typed(json_object)* anim_frames;

  result(json_element) frames_element_result = json_object_find(obj, "frames");
  if(result_is_err(json_element)(&frames_element_result)) {
    typed(json_error) error = result_unwrap_err(json_element)(&frames_element_result);
    fprintf(stderr, "Error getting element \"frames\": %s\n", json_error_to_string(error));
    return false;
  }
  typed(json_element) frames_element = result_unwrap(json_element)(&frames_element_result);
  anim_frames = frames_element.value.as_object;

  result(json_element) loop_element_result = json_object_find(obj, "loop");
  if(!result_is_err(json_element)(&loop_element_result)) {
    typed(json_element) loop_element = result_unwrap(json_element)(&loop_element_result);
    anim_loop = loop_element.value.as_boolean;
  }

  add_anim_asset(asset_name, anim_frames, anim_loop);

  return true;
}

bool init_assets(void) {
  result(json_element) element_result = json_parse(get_file_text("./assets.min.json"));

  // Guard if
  if(result_is_err(json_element)(&element_result)) {
    typed(json_error) error = result_unwrap_err(json_element)(&element_result);
    fprintf(stderr, "Error parsing JSON: %s\n", json_error_to_string(error));
    return false;
  }

  // Extract the data
  typed(json_element) element = result_unwrap(json_element)(&element_result);
  typed(json_object) *obj = element.value.as_object;

  for (int phase=0; phase<2; phase++) {
  for(int i = 0; i < obj->count; i++) {
    typed(json_entry) entry = *(obj->entries[i]);

    typed(json_string) key = entry.key;
    typed(json_element_type) type = entry.element.type;
    typed(json_element_value) value = entry.element.value;
    if (type == JSON_ELEMENT_TYPE_OBJECT) {
      switch (phase){
        case 0:
          process_json_sprite((const char*)key, entry.element.value.as_object);
        break;
        case 1:
          process_json_anim((const char*)key, entry.element.value.as_object);
        break;
      }
    }
    // Do something with `key`, `type` and `value`
  }
  }
  json_free(&element);
  return true;
}

void free_asset(Asset* a) {
  if (a->alias) {
    free(a->alias);
  }
  if (a->ui_figure) {
    free_ui_element(a->ui_figure);
  }
}

void free_all_assets() {
  for (int i=0; i < global_assets_count; i++) {
    free_asset(&global_assets[i]);
  } 
  free(global_assets);
}


