#ifndef UI_H
#define UI_H

#include <engine/geometry.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    UI_COLOR_WHITE=1,
    UI_COLOR_RED,
    UI_COLOR_BLUE
} UIColor;

typedef enum {
    FIGURE_POINT,
    FIGURE_RECTANGLE,
    FIGURE_BORDER,
		FIGURE_TEXT,
		FIGURE_SPRITE
} FigureType;

//bitflags
typedef enum {
    ALIGN_CENTER=1,
    ALIGN_LEFT=2,
    ALIGN_RIGHT=4,
    ALIGN_MIDDLE=8,
    ALIGN_TOP=16,
    ALIGN_BOTTOM=32,
} TextAlignType;

//Generic UI element struct so we can use functions for all UI elements.
typedef struct {
    FigureType type;
		UIColor color;
		bool visible;
} UIFigure;

typedef struct {
	//START: UIFigure elements: order DOES matter
    FigureType type;
		UIColor color;
		bool visible;
	//END
	
		Position pos;
		int ch;
} UIPoint;

typedef struct {
	//START: UIFigure elements: order DOES matter
    FigureType type;
		UIColor color;
		bool visible;
	//END

		Rect rect;
		int ch;
} UIRect;

typedef struct {
	//START: UIFigure elements: order DOES matter
    FigureType type;
		UIColor color;
		bool visible;
	//END

		Rect box;
		TextAlignType align;
		char* text;
} UIText;

typedef struct {
	//START: UIFigure elements: order DOES matter
    FigureType type;
		UIColor color;
		bool visible;
	//RECT

		Rect rect;
		int ch;
} UIBorder;

typedef struct {
	//START: UIFigure elements: order DOES matter
    FigureType type;
		UIColor color;
		bool visible;
	//RECT

    char* sprite;
    char* brightmap;
		Position pos;
} UISprite;


//Shorthand functions
UIPoint create_uipoint(int x, int y);
UIRect create_uirect(int x, int y, int width, int height);
UIBorder create_uiborder(int x, int y, int width, int height);
UIText create_uitext(Rect box, const char* text);
UISprite create_uisprite(int x, int y, const char* sprite);

#endif
