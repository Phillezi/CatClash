#ifndef GAME_H
#define GAME_H

#include "definitions.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include <pthread.h>
#include "../include/init.h"
#include "../include/text.h"
#include "../include/menu.h"
#include "../include/player.h"
#include "../include/levelEditor.h"
#include "../include/newClient.h"

Game *createGame();
int initGame(Game *pGame);
#ifdef SDL_MIXER_H_
bool loadMusic(Game *pGame);
#endif
void runGame(Game *pGame);
void closeGame(Game *pGame);
void *updateScreen(void *pGameIn);

#endif