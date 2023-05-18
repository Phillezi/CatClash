#ifndef TILE_H
#define TILE_H

#include "adts.h"

Tile *createTiles(int size);

void destroyTiles(Tile *pMap);

SDL_Texture **createTileTextures(App *pApp,int nrOfTextures);

void destroyTileTextures(SDL_Texture **ppTileTextures);

void loadTileAtlas(SDL_Renderer *pRenderer, SDL_Texture *pTiles[], const char imagePath[]);

SDL_Texture *loadTexture(SDL_Renderer *pRenderer, const char imagePath[]);

int initMap(Tile map[], char filepath[], int tileSize);

Tile createTile(int col, int row, int type, int tileSize);

#endif