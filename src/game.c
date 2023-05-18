#include "game.h"

Game *createGame(App *pApp)
{
    Game *pNew_game = (Game *)malloc(sizeof(Game));
    if (!pNew_game)
    {
        printf("Failed to allocate memory for game\n");
        return NULL;
    }
    if (initalizegame(pNew_game, pApp))
    {
        printf("Failed to initialize game\n");
        return NULL;
    }
    return pNew_game;
}

void destroyGame(Game *pGame)
{
    if(pGame->ppTileTextures)
        destroyTileTextures(pGame->ppTileTextures);

    if(pGame->pMap)
        destroyTiles(pGame->pMap);

    if(pGame->pPlayers)
        destroyPlayer(pGame->pPlayers);
        
    if(pGame)
        free(pGame);
}

int initalizegame(Game *pNew_game, App *pApp)
{
    pNew_game->tileSize = pApp->pWindow->width / 16;
    pNew_game->movementAmount = pNew_game->tileSize / 32;
    pNew_game->pPlayers = NULL;
    pNew_game->pPlayers = createNewPlayer(pNew_game, 0);
    if(!pNew_game->pPlayers)
    {
        printf("Error: could not create players\n");
        return 1;
    }

    pNew_game->pPlayers[0].rect.w = pNew_game->tileSize;
    pNew_game->pPlayers[0].rect.h = pNew_game->tileSize;


    pNew_game->pMap = createTiles((MAPSIZE*MAPSIZE));
    if(!pNew_game->pMap)
    {
        printf("Error: could not create map\n");
        return 1;
    }

    pNew_game->ppTileTextures = createTileTextures(pApp, TILES);
    if(!pNew_game->ppTileTextures)
    {
        printf("Error: could not create textures\n");
        return 1;
    }

    initMap(pNew_game->pMap, "map", pNew_game->tileSize);

    getPlayerSpawnPos(pNew_game);

    return 0;
}