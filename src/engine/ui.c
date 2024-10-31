#include <engine/ui.h>
#include <string.h>

UIPoint create_uipoint(int x, int y) {
	UIPoint point;
	point.type = FIGURE_POINT; 
	point.pos.x = x;
	point.pos.y = y;
	point.ch = '#';
	point.color = UI_COLOR_WHITE;
	point.visible = true;

	return point;
}

UIRect create_uirect(int x, int y, int width, int height) {
	UIRect ui_rect;
	ui_rect.type = FIGURE_RECTANGLE; 
	ui_rect.rect.pos.x = x;
	ui_rect.rect.pos.y = y;
	ui_rect.rect.size.width = width;
	ui_rect.rect.size.height = height;
	ui_rect.ch = '#';
	ui_rect.visible = true;
	ui_rect.color = UI_COLOR_WHITE;

	return ui_rect;
}

UIBorder create_uiborder(int x, int y, int width, int height) {
	UIBorder ui_border;
	ui_border.type = FIGURE_BORDER; 
	ui_border.rect.pos.x = x;
	ui_border.rect.pos.y = y;
	ui_border.rect.size.width = width;
	ui_border.rect.size.height = height;
	ui_border.ch = '#';
	ui_border.visible = true;
	ui_border.color = UI_COLOR_WHITE;

	return ui_border;
}

UIText create_uitext(Rect box, const char* text) {
	UIText ui_text;
	ui_text.type = FIGURE_TEXT; 
	ui_text.box = box;
	ui_text.text = calloc(strlen(text)+1, sizeof(char));
	strcpy(ui_text.text, text);
	ui_text.color = UI_COLOR_WHITE;
	ui_text.align = ALIGN_LEFT | ALIGN_TOP;
	ui_text.visible = true;

	return ui_text;
}

UISprite create_uisprite(int x, int y, const char* sprite) {
  UISprite ui_sprite;
  ui_sprite.type = FIGURE_SPRITE;
  ui_sprite.pos.x = x;
  ui_sprite.pos.y = y;
  ui_sprite.color = UI_COLOR_WHITE;
  ui_sprite.sprite = calloc(strlen(sprite)+1, sizeof(char));
  strcpy(ui_sprite.sprite, sprite);
  ui_sprite.brightmap = NULL;
  ui_sprite.visible = true;
  return ui_sprite;
}

UISpriteAnim create_uisprite_anim(int x, int y) {
  UISpriteAnim ui_sprite_anim;
  ui_sprite_anim.type = FIGURE_SPRITE_ANIM;
  ui_sprite_anim.pos.x = x;
  ui_sprite_anim.pos.y = y;
  ui_sprite_anim.color = UI_COLOR_WHITE;
  ui_sprite_anim.visible = true;
  ui_sprite_anim.sprites = NULL;
  ui_sprite_anim.delay = NULL;
  ui_sprite_anim.current_sprite=0;
  ui_sprite_anim.num_sprites=0;
  ui_sprite_anim.wait=0;
  ui_sprite_anim.loop = true;

  return ui_sprite_anim;
 
}
void add_sprite_to_anim(UISpriteAnim* anim, UISprite* sprite, int delay) {
  anim->num_sprites++;
  anim->sprites = realloc(anim->sprites, sizeof(UISprite)*anim->num_sprites);
  anim->delay = realloc(anim->delay, sizeof(int)*anim->num_sprites);
  anim->sprites[anim->num_sprites-1] = *sprite;
  anim->delay[anim->num_sprites-1] = delay;
}

