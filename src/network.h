#ifndef NETWORK_H
#define NETWORK_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL_net.h>
#include "adts.h"

Network *createNetwork();

void destroyNetwork(Network *pNetwork);

int initalizenetwork(Network *pNew_network);

#endif