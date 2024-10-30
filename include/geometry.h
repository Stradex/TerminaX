#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	int width;
	int height;
} Size;

typedef struct {
	Size size;
	Point pos;
} Rect;

#endif
