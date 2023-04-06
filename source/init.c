#include "init.h"

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
        //DEBUG printf("TEXTURE %d\n", tile);
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
int initTexturePlayer(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer){
    
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
    expected in-parameters: Tile map[]
*/
void initMap(Tile map[])
{
    int type = 0;
    // READ FROM map.txt
    FILE *fp;
    fp = fopen(MAP_FILEPATH, "r");
    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                type = 0;
                fscanf(fp, "%d", &type);
                map[row * MAPSIZE + col] = createTile(col * TILESIZE, row * TILESIZE, type);
            }
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR READING FILE");
    }
}
/*
    createTile:
    creates a tile
    expected in-parameters: int x, int y, int type
*/
Tile createTile(int x, int y, int type)
{
    Tile i;
    i.wall.x = x;
    i.wall.y = y;
    i.wall.w = TILESIZE;
    i.wall.h = TILESIZE;
    i.type = type;
    return i;
}