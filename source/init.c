#include "init.h"
#include <string.h>

/*
    initTextureTiles:
    initializes textures for tiles
    Expected in-parameters: pRenderer, pWindow, pTextureTiles, tileTextures
    Returns -1 if it fails
*/
int initTextureTiles(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture *pTextureTiles[], char tileTextures[][PATH_LEN], int nrOfTiles)
{

    for (int tile = 0; tile < nrOfTiles; tile++)
    {
        // DEBUG printf("TEXTURE %d\n", tile);
        SDL_Surface *pSurface1 = IMG_Load(tileTextures[tile]);
        if (!pSurface1)
        {
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
    initTexturePlayer:
    initializes textures for player
    Expected in-parameters: pRenderer, pWindow, &pTexturePlayer
    Returns -1 if it fails
*/
int initTexturePlayer(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer)
{

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
/*
    initMap:
    Reads the mapfile and fills the Tile map array
    expected in-parameters: Tile map[], char filepath[], int tileSize
*/
int initMap(Tile map[], char filepath[], int tileSize)
{
    int type = 0;
    char location[100];
    FILE *fp;

    if (strstr(filepath, ".txt"))
        sprintf(location, "%s%s", SAVE_MAP_PATH, filepath);
    else
        sprintf(location, "%s%s.txt", SAVE_MAP_PATH, filepath);

    if (!filepath[0])
    {
        // READ FROM map.txt
        fp = fopen(MAP_FILEPATH, "r");
        printf("Reading fromn map.txt\n");
    }
    else
    {
        fp = fopen(location, "r");
    }

    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                type = 0;
                fscanf(fp, "%d", &type);
                map[row * MAPSIZE + col] = createTile(col, row, type, tileSize);
                map[row * MAPSIZE + col].x = map[row * MAPSIZE + col].wall.x;
                map[row * MAPSIZE + col].y = map[row * MAPSIZE + col].wall.y;
            }
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR READING FILE\n");
        return -1;
    }
    return 0;
}
/*
    createTile:
    creates a tile
    expected in-parameters: int x, int y, int type, int tileSize
*/
Tile createTile(int col, int row, int type, int tileSize)
{
    Tile i;
    i.wall.x = col * tileSize;
    i.wall.y = row * tileSize;
    i.wall.w = tileSize;
    i.wall.h = tileSize;
    i.type = type;
    return i;
}

int readConfig(Config *pConfig){

    FILE *fp;

    fp = fopen("resources/config.txt", "r");

    if (fp != NULL)
    {
        //kod som kollar igenom config.txt och uppdaterar config structen därefter
        fclose(fp);
    }
    else
    {
        printf("ERROR READING CONFIG FILE\n");
        return -1;
    }
    return 0;
}