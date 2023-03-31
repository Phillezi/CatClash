#ifndef INIT_H
#define INIT_H
#define PATH_LEN 20
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int initTextureTiles(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture *pTextureTiles[], char tileTextures[][PATH_LEN]);

int initTexturePlayer(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer);

#endif