#ifndef INIT_H
#define INIT_H
#define PATH_LEN 20
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "definitions.h"

int initTextureTiles(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture *pTextureTiles[], char tileTextures[][PATH_LEN], int nrOfTiles);

int initTexturePlayer(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer);

int initMap(Tile map[], char filepath[], int tileSize);

Tile createTile(int x, int y, int type, int tileSize);

#endif