#include <engine/assets_parser.h>
#include <util/file_manager.h>
#include <engine/ui.h>

UISprite load_sprite_from_file(const char* file, const char* brightmap_file) {
  UISprite sprite = create_uisprite(0, 0, get_file_text(file));
  if (brightmap_file) {
    sprite.brightmap = (char *)get_file_text(brightmap_file);
  }
  return sprite;
}
