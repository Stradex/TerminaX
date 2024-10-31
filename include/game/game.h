#ifndef GAME_H
#define GAME_H

#include <engine/renderer.h>
#include <stdio.h>

typedef struct {
	Renderer* renderer;
	void (*game_logic)();
} Game;

Game* create_game(int width, int height, void (*game_logic)());
void game_start(Game* game);
#endif
