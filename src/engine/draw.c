#include <engine/draw.h>
#include <engine/geometry.h>
#include <ncurses.h>
#include <string.h>

Position get_draw_position(Position pos, Renderer* renderer) {
	Position new_pos;
	int pos_mult = renderer->screen.size.width / renderer->size.width;
	new_pos.x = renderer->screen.pos.x + pos.x*pos_mult;
	new_pos.y = renderer->screen.pos.y + pos.y*pos_mult;

	return new_pos;
}

Rect get_draw_rect(Rect rect, Renderer* renderer) {
	Rect new_rect;
	new_rect.pos = get_draw_position(rect.pos, renderer);

	int pos_mult = renderer->screen.size.width / renderer->size.width;
	
	new_rect.size.width = rect.size.width*pos_mult;
	new_rect.size.height = rect.size.height*pos_mult;

	return new_rect;
}


void draw_element(void* e, Renderer* renderer) {
	UIFigure *figure = (UIFigure *)e;

	if (figure->visible == 0) {
		return;
	}

	switch (figure->type) {
		case FIGURE_POINT:
			draw_point((UIPoint*)e, renderer);
		break;
		case FIGURE_RECTANGLE:
			draw_rect((UIRect*)e, renderer);
		break;
		case FIGURE_BORDER:
			draw_border((UIBorder*)e, renderer);
		break;
		case FIGURE_TEXT:
			draw_text((UIText*)e, renderer);
		break;
		case FIGURE_SPRITE:
			draw_sprite((UISprite*)e, renderer);
		break;

	}
}

void draw_char(Position pos, int ch, int color, Renderer* renderer) {
	Position draw_pos = get_draw_position(pos, renderer);
	mvaddch(draw_pos.y, draw_pos.x, ch | COLOR_PAIR(color));
}


void draw_point(UIPoint* p, Renderer* renderer) {
	Position draw_pos = get_draw_position(p->pos, renderer);
	int size_mult = renderer->screen.size.width / renderer->size.width;

	for (int i=0; i < size_mult; i++) {
		for (int j=0; j < size_mult; j++) {
			mvaddch(draw_pos.y+i, draw_pos.x+j, p->ch | COLOR_PAIR(p->color));
		}
	}
}

void draw_border(UIBorder* r, Renderer* renderer) {

	for(int i=0; i <= r->rect.size.width; i++) {
		for(int j=0; j <= r->rect.size.height; j++) {
			if (i!=0 && i != r->rect.size.width && j!= 0 && j != r->rect.size.height) {
				continue;
			}
			UIPoint p = create_uipoint(i+r->rect.pos.x, j+r->rect.pos.y);
			p.color = r->color;
			p.ch = r->ch;
			draw_point(&p, renderer);
		}
	}
}


void draw_rect(UIRect* r, Renderer* renderer) {

	for(int i=0; i <= r->rect.size.width; i++) {
		for(int j=0; j <= r->rect.size.height; j++) {
			UIPoint p = create_uipoint(i+r->rect.pos.x, j+r->rect.pos.y);
			p.color = r->color;
			p.ch = r->ch;
			draw_point(&p, renderer);
		}
	}
}


void draw_text(UIText* e, Renderer* renderer) {
	int text_len = strlen(e->text);
	for (int i=0; i < text_len; i++) {
		int x	= i;
		int y = 0;

		if (e->align & ALIGN_BOTTOM) {
			y += e->box.pos.y + e->box.size.height;
		} else if (e->align & ALIGN_MIDDLE) {
			y += (e->box.pos.y + e->box.size.height)/2;
		} else {
			y += e->box.pos.y;
		}

		if (e->align & ALIGN_RIGHT) {
			x += e->box.pos.x + e->box.size.width - text_len;
		} else if (e->align & ALIGN_CENTER) {
			x += (e->box.pos.x + e->box.size.width - text_len)/2;
		} else {
			x += e->box.pos.x;
		}

		draw_char(create_pos(x, y), e->text[i], e->color, renderer);
	}
}


void draw_sprite(UISprite* s, Renderer* renderer) {

  int len = strlen(s->sprite);
  for (int i=0, col=0, row=0; i < len; i++, col++) {
    if (s->sprite[i] == ' ' || s->sprite[i] == '\t' || s->sprite[i] == '\r' ) {
     continue; 
    }
    if (s->sprite[i] == '\n') {
      row++;
      col=-1;
      continue;
    }
    int x=col + s->pos.x;
    int y=row + s->pos.y;
    UIPoint p = create_uipoint(x, y);
    p.color = s->brightmap ? (UIColor)(s->brightmap[i] - '0') : s->color;
    p.ch = s->sprite[i];
    draw_point(&p, renderer);
  }
}
