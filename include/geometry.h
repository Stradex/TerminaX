#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct {
	int x;
	int y;
} Position;

typedef struct {
	int width;
	int height;
} Size;

typedef struct {
	Size size;
	Position pos;
} Rect;

Rect create_rect(int x, int y, int width, int height);
Position create_pos(int x, int y);

#endif
