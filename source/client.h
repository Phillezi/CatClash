#ifndef CLIENT_H
#define CLIENT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>

int clientTCP(char serverIP[]);
int tryConnectTcp(char serverIP[]);
void *readMessage(void *client);

#endif