#include <game.h>
#include <ncurses.h>

void init_colors(void) {
	if (has_colors())
	{
		start_color();
		init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
		init_pair(BORDER_COLOR, COLOR_RED, COLOR_BLACK);
	}
}

void init_ncurses(void) {
	initscr();
	noecho();
	curs_set(0);
	halfdelay(4);
	init_colors();
}

void refresh_game_size(Game* game) {
	update_terminal(&game->t);
	Rect screen;
	screen.pos.x = 2;
	screen.pos.y = 2;
	screen.size.width = game->t.cols - screen.pos.x - 2;
	screen.size.height = game->t.rows - screen.pos.y - 2;
	game->screen = screen;
	printf("game width: %d\n", game->screen.size.width);
}

Game* init_game() {
	init_ncurses();
	Game *game = calloc(1, sizeof(Game));
	game->t = *(get_current_terminal());
	refresh_game_size(game);
	return game;
}	

void game_render(Game* game, int ch) {
	for (int i = 0; i <= game->screen.size.width; i++) {
		for (int j = 0; j <= game->screen.size.height; j++) {
			if (i!=0 && i != game->screen.size.width && j!=0 && j!= game->screen.size.height) {
				continue;
			}
			int x = i + game->screen.pos.x;
			int y = j + game->screen.pos.y;
			mvaddch(y, x, '#' | COLOR_PAIR(BORDER_COLOR));
		}
	}
}


void game_loop(Game* game) {
	int ch = 0;
 printf("Width: %d\n", game->screen.size.width);
	do {
		timeout(1);
		ch = getch();
		clear();
		refresh_game_size(game);
		game_render(game, ch);
		refresh();
	} while(ch != 'q');
}

void game_end(Game* game) {
	endwin();
}
