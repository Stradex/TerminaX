#include <engine/renderer.h>
#include <engine/draw.h>
#include <engine/ui.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ncurses.h>
#include <stdio.h>

int frame_number = 0;

void init_colors(void) {
	if (has_colors())
	{
		start_color();
		init_pair(UI_COLOR_WHITE, COLOR_WHITE, -1);
		init_pair(UI_COLOR_RED, COLOR_RED, -1);
		init_pair(UI_COLOR_BLUE, COLOR_BLUE, -1);
	}
}

void init_ncurses(void) {
	initscr();
	noecho();
	curs_set(0);
	halfdelay(4);
	use_default_colors();
	init_colors();
}


bool can_render(Renderer* render) {
	int scale_x = (render->t.cols - 4)/render->size.width;
	int scale_y = (render->t.rows - 4)/render->size.height;
	return scale_x > 0 && scale_y > 0;
}

void refresh_renderer_size(Renderer* render) {
	update_terminal(&render->t);
	Rect screen;

	int scale_x = (render->t.cols - 4)/render->size.width;
	int scale_y = (render->t.rows - 4)/render->size.height;
	int scale = (scale_x > scale_y) ? scale_y : scale_x;

	screen.size.width = render->size.width*scale;
	screen.size.height = render->size.height*scale;
	screen.pos.x = (render->t.cols - screen.size.width)/2;
	screen.pos.y = (render->t.rows - screen.size.height)/2-1;
	render->screen = screen;
}

Renderer* init_renderer(int width, int height) {
	init_ncurses();
	Renderer *renderer = calloc(1, sizeof(Renderer));
	renderer->t = *(get_current_terminal());
	renderer->size.width = width;
	renderer->size.height = height;
	refresh_renderer_size(renderer);
	return renderer;
}	

void draw_renderer_border(Renderer* renderer) {
	UIBorder r = create_uiborder(-1, -1,renderer->size.width+2, renderer->size.height+2);
	r.color = UI_COLOR_RED;
	draw_element(&r, renderer);
}

void engine_render(Renderer* renderer, void (*game_logic)()) {
	if (!can_render(renderer)) {
			const char* msg = "INCREASE TERMINAL SIZE";
			int msg_len = strlen(msg);
			for(int i=0; i < msg_len; i++) {
				mvaddch(2, i+2, msg[i] | COLOR_PAIR(UI_COLOR_RED));
			}
			
			return;
	}
	game_logic();

	frame_number++;
}


void renderer_loop(Renderer* renderer, void (*game_logic)()) {
	int ch = 0;
	do {
		timeout(1);
		ch = getch();
		clear();
		refresh_renderer_size(renderer);
		engine_render(renderer, game_logic);
		draw_renderer_border(renderer);
		refresh();
	} while(ch != 'q');
}

void renderer_end(Renderer* renderer) {
	endwin();
}

char *va( const char *fmt, va_list argptr) {
	static int index = 0;
	static char string[4][16384];
	char *buf;

	buf = string[index];
	index = (index + 1) & 3;

	vsprintf( buf, fmt, argptr );

	return buf;
}

void engine_print(const char* txt, Renderer* renderer, ...) {

	va_list args;
	va_start(args, renderer);
	const char* print_str = va(txt, args);
	va_end(args);

	int txt_len = strlen(print_str);
	for (int i=0; i < txt_len; i++) {
			mvaddch(renderer->t.rows-1, i+1, print_str[i] | COLOR_PAIR(UI_COLOR_BLUE));
	}
}
