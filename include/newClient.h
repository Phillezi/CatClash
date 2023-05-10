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
void *scanForGamesOnLocalNetwork(void *arg);
void getLocalAddresses(IPaddress *pAddresses, int *pAddress_count, int max_addresses);
void ipToString(IPaddress *ip, char *ip_str);

//UDP

void sendData(Game *pGame);
void getPlayerData(Game *pGame);



#endif