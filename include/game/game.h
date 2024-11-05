#ifndef GAME_H
#define GAME_H

#include <engine/renderer.h>
#include <engine/networking.h>
#include <stdio.h>


typedef struct {
	Renderer* renderer;
  NetworkSettings* net_config;
	void (*game_logic)();
} Game;
Game* create_game(int width, int height, void (*game_logic)(), int argc, char* argv[]);
void game_start(Game* game);
#endif
