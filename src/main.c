#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <game.h>

Game* current_game;

int main (void) {
	current_game = init_game();
	game_loop(current_game);
	game_end(current_game);
	return 0;
}
