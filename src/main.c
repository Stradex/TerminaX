#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <game.h>

Game* current_game;

int main (void) {
	current_game = init_game(60, 15); //fixed size: 15 cols - 30 rows
	game_loop(current_game);
	game_end(current_game);
	return 0;
}
