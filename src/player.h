#ifndef PLAYER_H
#define PLAYER_H

#include "adts.h"

Player *createNewPlayer(Game *pGame, int size);

void destroyPlayer(Player *pPlayers);


#endif