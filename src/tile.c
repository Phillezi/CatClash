#include "tile.h"

Tile *createTiles(int size)
{
    Tile *pNew_Tiles = malloc(size *sizeof(Tile));
    if(!pNew_Tiles)
    {
        printf("Error: Could not allocate memory for tile array\n");
        return NULL;
    }
    return pNew_Tiles;
}

void destroyTiles(Tile *pMap)
{
    free(pMap);
}