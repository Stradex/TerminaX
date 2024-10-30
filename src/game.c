#include <game.h>
#include <draw.h>
#include <ui.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ncurses.h>

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


bool can_game_render(Game* game) {
	int scale_x = (game->t.cols - 4)/game->size.width;
	int scale_y = (game->t.rows - 4)/game->size.height;
	return scale_x > 0 && scale_y > 0;
}

void refresh_game_size(Game* game) {
	update_terminal(&game->t);
	Rect screen;

	int scale_x = (game->t.cols - 4)/game->size.width;
	int scale_y = (game->t.rows - 4)/game->size.height;
	int scale = (scale_x > scale_y) ? scale_y : scale_x;

	screen.size.width = game->size.width*scale;
	screen.size.height = game->size.height*scale;
	screen.pos.x = (game->t.cols - screen.size.width)/2;
	screen.pos.y = (game->t.rows - screen.size.height)/2-1;
	game->screen = screen;
}

Game* init_game(int width, int height) {
	init_ncurses();
	Game *game = calloc(1, sizeof(Game));
	game->t = *(get_current_terminal());
	game->size.width = width;
	game->size.height = height;
	refresh_game_size(game);
	return game;
}	

void draw_game_border(Game* game) {
	UIBorder r = create_uiborder(-1, -1, game->size.width+2, game->size.height+2);
	r.color = UI_COLOR_RED;
	draw_element(&r, game);
}

void game_render(Game* game, int ch) {
	if (!can_game_render(game)) {
			const char* msg = "INCREASE TERMINAL SIZE";
			int msg_len = strlen(msg);
			for(int i=0; i < msg_len; i++) {
				mvaddch(2, i+2, msg[i] | COLOR_PAIR(UI_COLOR_RED));
			}
			
			return;
	}
	//test draw point
	game_print("frame: %d", game, frame_number);

	frame_number++;
}


void game_loop(Game* game) {
	int ch = 0;
	do {
		timeout(1);
		ch = getch();
		clear();
		refresh_game_size(game);
		game_render(game, ch);
		draw_game_border(game);
		refresh();
	} while(ch != 'q');
}

void game_end(Game* game) {
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

void game_print(const char* txt, Game* game, ...) {

	va_list args;
	va_start(args, game);
	const char* print_str = va(txt, args);
	va_end(args);

	int txt_len = strlen(print_str);
	for (int i=0; i < txt_len; i++) {
			mvaddch(game->t.rows-1, i+1, print_str[i] | COLOR_PAIR(UI_COLOR_BLUE));
	}
}
