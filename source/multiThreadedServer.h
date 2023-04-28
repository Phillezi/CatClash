#ifndef MULTITHREADEDSERVER_H
#define MULTITHREADEDSERVER_H

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include "definitions.h"
#include <pthread.h>
#include "init.h"
#include "text.h"

#define MAX_PLAYERS 5
#define PORT 1234

void *MThostServer(void *map);
void debugPrint();
int MTsetup(Server *pServer);

int MTsetupTCP(Server *pServer);
void *MTtcpServer(void *pServerIn);

int MTsetupUDP(Server *pServer);
void MTcheckUdpClient(Server *pServer, Player data);
void *MTudpServer(void *pServerIn);

void MTupdateServerScreen(Server *pServer);

void MTclose(Server *pServer);

#endif