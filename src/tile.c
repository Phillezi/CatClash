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

SDL_Texture **createTileTextures(App *pApp,int nrOfTextures)
{
    SDL_Texture **ppTextures = (SDL_Texture **)malloc(nrOfTextures * sizeof(SDL_Texture *));
    if(!ppTextures)
    {
        printf("Error: Could not allocate memory for textures\n");
        return NULL;
    }
    loadTileAtlas(pApp->pWindow->pRenderer, ppTextures, "resources/tileMap.png");
    if(!ppTextures)
    {
        printf("Error: Could not create textures\n");
        return NULL;
    }
    return ppTextures;
}

void destroyTileTextures(SDL_Texture **ppTileTextures)
{
    for(int i = 0; i < TILES; i++)
        SDL_DestroyTexture(ppTileTextures[i]);
    free(ppTileTextures);
}

/*
    loadTileAtlas:
    loads texture from png image, then divides texture to tiles
    Expected in-parameters: pRenderer, pTileTextures, imgpath
*/
void loadTileAtlas(SDL_Renderer *pRenderer, SDL_Texture *pTiles[], const char imagePath[])
{
    SDL_Texture *pTexture = loadTexture(pRenderer, imagePath);
    if (!pTexture)
    {
        printf("Failed to load tile atlas %s\n", imagePath);
        return;
    }

    int atlasWidth, atlasHeight;
    SDL_QueryTexture(pTexture, NULL, NULL, &atlasWidth, &atlasHeight);
    int row = 0;
    int offset = 0;
    for (int i = 0; i < TILES; i++)
    {
        if ((i * TILE_WIDTH) - (row * atlasWidth) > atlasWidth)
        {
            row++;
        }
        int x = (i * TILE_WIDTH + offset) - (row * atlasWidth);
        int y = row * TILE_HEIGHT;
        SDL_Rect srcRect = {x, y, TILE_WIDTH, TILE_HEIGHT};
        pTiles[i] = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, TILE_WIDTH, TILE_HEIGHT);
        if (!pTiles[i])
        {
            offset += TILE_WIDTH;
            i--;
        }
        SDL_SetRenderTarget(pRenderer, pTiles[i]);
        SDL_RenderCopy(pRenderer, pTexture, &srcRect, NULL);
        SDL_SetRenderTarget(pRenderer, NULL);
    }

    SDL_DestroyTexture(pTexture);
}

/*
    loadTexture:
    loads texture from png image
    Expected in-parameters: pRenderer, imagePath
    Returns NULL if it fails
*/
SDL_Texture *loadTexture(SDL_Renderer *pRenderer, const char imagePath[])
{
    SDL_Surface *pSurface = IMG_Load(imagePath);
    if (!pSurface)
    {
        printf("Failed to load image %s: %s\n", imagePath, IMG_GetError());
        return NULL;
    }

    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    if (!pTexture)
    {
        printf("Failed to create texture from %s: %s\n", imagePath, SDL_GetError());
        SDL_FreeSurface(pSurface);
        return NULL;
    }

    SDL_FreeSurface(pSurface);
    return pTexture;
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
        sprintf(location, "%s%s", "resources/maps/", filepath);
    else
        sprintf(location, "%s%s.txt", "resources/maps/", filepath);

    if (!filepath[0])
    {
        // READ FROM map.txt
        fp = fopen("resources/maps/map.txt", "r");
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
                map[row * MAPSIZE + col].x = map[row * MAPSIZE + col].rect.x;
                map[row * MAPSIZE + col].y = map[row * MAPSIZE + col].rect.y;
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
    i.rect.x = col * tileSize;
    i.rect.y = row * tileSize;
    i.rect.w = tileSize;
    i.rect.h = tileSize;
    i.type = type;
    return i;
}