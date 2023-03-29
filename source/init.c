#include "init.h"

/*
    initTiles
    Expected in-parameters: pRenderer, pWindow, pTextureTiles, tileTextures
    Returns -1 if it fails
*/
int initTiles(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture *pTextureTiles[], char tileTextures[][PATH_LEN])
{

    for (int tile = 0; tile < sizeof(*pTextureTiles)/sizeof(int); tile++)
    {
        SDL_Surface *pSurface1 = IMG_Load(tileTextures[tile]);
        if (!pSurface1)
        {
            printf("%s failed\n",tileTextures[tile]);
            return -1;
        }

        pTextureTiles[tile] = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
        SDL_FreeSurface(pSurface1);
        if (!pTextureTiles[tile])
        {
            return -1;
        }
    }
    return 0;
}

/*
    initPlayer
    Expected in-parameters: pRenderer, pWindow, &pTexturePlayer
    Returns -1 if it fails
*/
int initPlayer(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer){
    
    SDL_Surface *pSurface = IMG_Load("resources/cat3.png");
    if (!pSurface)
    {
        return -1;
    }

    *pTexturePlayer = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if (!pTexturePlayer)
    {
        return -1;
    }
    return 0;
}
