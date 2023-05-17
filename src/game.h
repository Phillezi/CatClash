#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <stdio.h>

#include "adts.h"

Game *createGame();

void destroyGame(Game *pGame);

int initalizegame(Game *pNew_game);

#endif