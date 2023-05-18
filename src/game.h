#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdio.h>

#include "adts.h"
#include "player.h"
#include "tile.h"

Game *createGame(App *pApp);

void destroyGame(Game *pGame);

int initalizegame(Game *pNew_game, App *pApp);

#endif