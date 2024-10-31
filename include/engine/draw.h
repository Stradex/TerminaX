#ifndef DRAW_H
#define DRAW_H

#include <engine/ui.h>
#include <engine/renderer.h>

void draw_element(void* e, Renderer* renderer);
void draw_point(UIPoint* p, Renderer* renderer);
void draw_rect(UIRect* r, Renderer* renderer);
void draw_border(UIBorder* d, Renderer* renderer);
void draw_text(UIText* e, Renderer* renderer);
void draw_sprite(UISprite* s, Renderer* renderer);

#endif
