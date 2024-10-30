#include <ui.h>
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
