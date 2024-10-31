#ifndef UI_H
#define UI_H

#include <engine/geometry.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    UI_COLOR_WHITE=1,
    UI_COLOR_RED, //2
    UI_COLOR_BLUE, //3
    UI_COLOR_GREEN, //4
    UI_COLOR_YELLOW, //5
    UI_COLOR_MAGENTA, //6
    UI_COLOR_CYAN, //7
    UI_COLOR_BLACK //8
} UIColor;

typedef enum {
    FIGURE_POINT,
    FIGURE_RECTANGLE,
    FIGURE_BORDER,
		FIGURE_TEXT,
		FIGURE_SPRITE,
		FIGURE_SPRITE_ANIM
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

typedef struct {
	//START: UIFigure elements: order DOES matter
    FigureType type;
		UIColor color;
		bool visible;
	//RECT
  
    UISprite* sprites;
    int* delay; //delay of each sprite frame
    int num_sprites;
    int current_sprite;
    int wait;
    bool loop;
    Position pos;
} UISpriteAnim;


//Shorthand functions
UIPoint create_uipoint(int x, int y);
UIRect create_uirect(int x, int y, int width, int height);
UIBorder create_uiborder(int x, int y, int width, int height);
UIText create_uitext(Rect box, const char* text);
UISprite create_uisprite(int x, int y, const char* sprite);
UISpriteAnim create_uisprite_anim(int x, int y);
void add_sprite_to_anim(UISpriteAnim* anim, UISprite* sprite, int delay);

#endif
