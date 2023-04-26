#ifndef CLIENTUDP_H
#define CLIENTUDP_H

#include "definitions.h"
#include <SDL2/SDL_net.h>

void sendData(Game *pGame);
void retrieveData(Game *pGame);
int getPlayerData(Game *pGame);

#endif