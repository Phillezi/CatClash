#ifndef NEWCLIENT_H
#define NEWCLIENT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>
#include "definitions.h"

int connectToServer(Game *pGame);
int initTCPConnection(Game *pGame);
void checkTCP(Game *pGame);
PlayerNet *createClient(char *serverIP, int port, int id, int x, int y);

//UDP

void sendData(Game *pGame);
void getPlayerData(Game *pGame);



#endif