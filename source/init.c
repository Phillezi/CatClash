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

/*  Old initTexturePlayer function
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
*/

int initTexturePlayer(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer)
{

    SDL_Rect srcRect;
    srcRect.x = 611; // test img X starting point
    srcRect.y = 485; // test img Y starting point
    srcRect.w = 24;
    srcRect.h = 24;

    SDL_Surface *pSurface = IMG_Load("resources/cat3.png");
    if (!pSurface)
    {
        return -1;
    }

    SDL_Surface *pCroppedSurface = SDL_CreateRGBSurface(0, 24, 20, pSurface->format->BitsPerPixel,
                                                        pSurface->format->Rmask, pSurface->format->Gmask,
                                                        pSurface->format->Bmask, pSurface->format->Amask);
    SDL_BlitSurface(pSurface, &srcRect, pCroppedSurface, NULL);
    SDL_FreeSurface(pSurface);

    *pTexturePlayer = SDL_CreateTextureFromSurface(pRenderer, pCroppedSurface);
    SDL_FreeSurface(pCroppedSurface);

    if (!pTexturePlayer)
    {
        return -1;
    }
    return 0;
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

int readConfig(Config *pConfig)
{

    FILE *fp;

    fp = fopen("resources/config.txt", "r");

    if (fp != NULL)
    {
        // kod som kollar igenom config.txt och uppdaterar config structen därefter
        fclose(fp);
    }
    else
    {
        printf("ERROR READING CONFIG FILE\n");
        return -1;
    }
    return 0;
}