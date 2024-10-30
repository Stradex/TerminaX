#ifndef DRAW_H
#define DRAW_H

#include <engine/ui.h>
#include <engine/game.h>

void draw_element(void* e, Game* game);
void draw_point(UIPoint* p, Game* game);
void draw_rect(UIRect* r, Game* game);
void draw_border(UIBorder* d, Game* game);
void draw_text(UIText* e, Game* game);

#endif
