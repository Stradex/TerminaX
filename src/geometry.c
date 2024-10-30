#include <geometry.h>

Position create_pos(int x, int y) {
	Position p;
	p.x = x;
	p.y = y;
	return p;
}

Rect create_rect(int x, int y, int width, int height) {
	Rect r;
	r.pos = create_pos(x, y);
	r.size.width = width;
	r.size.height = height;

	return r;
}
