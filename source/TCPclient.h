#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "definitions.h"

int ANVEJservInit(Game *pGame);
int joinServerTCP(Game *pGame);
PlayerNet *createClient(char *serverIP, int port, int id, int x, int y);

#endif