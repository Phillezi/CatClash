#include "player.h"

Player *createNewPlayer(Game *pGame, int size)
{
    Player *pNew_arr;
    if (pGame->pPlayers)
    {
        printf("Expanding memory for player array\n");
        pNew_arr = (Player *)realloc(pGame->pPlayers, (size + 1) * sizeof(Player));
    }
    else
    {
        pNew_arr = (Player *)malloc((size + 1) * sizeof(Player));
    }

    if (pNew_arr == NULL)
    {
        printf("ERROR when allocating new memory for joined player\n");
        return NULL;
    }

    return pNew_arr;
}

Player *removePlayer(App *pApp, int size)
{
    if (pApp->pGame->pPlayers == NULL)
    {
        printf("ERROR: Player array is empty\n");
        return NULL;
    }

    printf("Shrinking memory for player array\n");
    Player *pNew_arr = (Player *)realloc(pApp->pGame->pPlayers, size * sizeof(Player));
    if (pNew_arr == NULL && size > 0)
    {
        printf("ERROR when reallocating memory for player array\n");
        return NULL;
    }


    pApp->pGame->pPlayers = pNew_arr;
    return pNew_arr;
}

void destroyPlayer(Player *pPlayers)
{
    free(pPlayers);
}

void centerPlayer(App *pApp, Player *pPlayer)
{
    int screenShiftAmount = pApp->pGame->movementAmount;

    bool playerIsInOneFifthOfScreenBorder = (pPlayer->rect.x >= (4 * pApp->pWindow->width) / 5 || pPlayer->rect.x <= pApp->pWindow->width / 5) || (pPlayer->rect.y >= (4 * pApp->pWindow->height) / 5 || pPlayer->rect.y <= pApp->pWindow->height / 5);
    bool playerIsOutsideScreen = (pPlayer->rect.x >= pApp->pWindow->width || pPlayer->rect.x <= 0) || (pPlayer->rect.y >= pApp->pWindow->height || pPlayer->rect.y <= 0);
    bool playerIsMoreThanOneTileOutsideOfScreen = (pPlayer->rect.x >= pApp->pWindow->width + pApp->pGame->tileSize || pPlayer->rect.x <= -pApp->pGame->tileSize) || (pPlayer->rect.y >= pApp->pWindow->height + pApp->pGame->tileSize || pPlayer->rect.y <= -pApp->pGame->tileSize);
    bool playerIsCloseToUpperBorder = pPlayer->rect.y < (2 * pApp->pWindow->height) / 5;
    bool playerIsCloseToLowerBorder = pPlayer->rect.y > (3 * pApp->pWindow->height) / 5;
    bool playerIsCloseToLeftBorder = pPlayer->rect.x < (2 * pApp->pWindow->width) / 5;
    bool playerIsCloseToRigthBorder = pPlayer->rect.x > (3 * pApp->pWindow->width) / 5;

    if (playerIsMoreThanOneTileOutsideOfScreen)
        screenShiftAmount = pApp->pGame->movementAmount * 30;
    else if (playerIsOutsideScreen)
        screenShiftAmount = pApp->pGame->movementAmount * 20;
    else if (playerIsInOneFifthOfScreenBorder)
        screenShiftAmount = pApp->pGame->movementAmount * 2;

    if (playerIsCloseToUpperBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pApp->pGame->pMap[i].rect.y += screenShiftAmount;
        }
    }
    if (playerIsCloseToLowerBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pApp->pGame->pMap[i].rect.y -= screenShiftAmount;
        }
    }
    if (playerIsCloseToLeftBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pApp->pGame->pMap[i].rect.x += screenShiftAmount;
        }
    }
    if (playerIsCloseToRigthBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pApp->pGame->pMap[i].rect.x -= screenShiftAmount;
        }
    }
}

void movePlayer(Player *pPlayer, char direction)
{
    if (1)
    {
        switch (direction)
        {
        case 'W':
            pPlayer->y--;
            break;
        case 'A':
            pPlayer->x--;
            break;
        case 'S':
            pPlayer->y++;
            break;
        case 'D':
            pPlayer->x++;
            break;
        default:
            break;
        }
    }
    //pPlayer->prevKeyPressed = direction;
}

int checkCollision(Player player, Tile map[], char direction, int tileSize)
{
    switch (direction)
    {
    case 'W':
        if (map[(((player.y - 1) / tileSize) * MAPSIZE) + (player.x / tileSize)].type > 0)
            return map[(((player.y - 1) / tileSize) * MAPSIZE) + (player.x / tileSize)].type;
        else if (map[(((player.y - 1) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type > 0)
            return map[(((player.y - 1) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type;
        break;
    case 'A':
        if (map[(((player.y) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type > 0)
            return map[(((player.y) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type;
        else if (map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type > 0)
            return map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type;
        break;
    case 'S':
        if (map[(((player.y + tileSize) / tileSize) * MAPSIZE) + (player.x / tileSize)].type > 0)
            return map[(((player.y + tileSize) / tileSize) * MAPSIZE) + (player.x / tileSize)].type;
        else if (map[(((player.y + tileSize) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type > 0)
            return map[(((player.y + tileSize) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type;
        break;
    case 'D':
        if (map[(((player.y) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type > 0)
            return map[(((player.y) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type;
        else if (map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type > 0)
            return map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type;
        break;
    default:
        break;
    }

    return 0;
}

SDL_Rect findEmptyTile(Tile map[])
{
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (!map[i].type)
        {
            return map[i].rect;
        }
    }
}

SDL_Rect findSpawnTile(Tile map[])
{
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (map[i].type == -1)
        {
            return map[i].rect;
        }
    }
    printf("Error: No spawn Tile in Selected map!\n");
    return findEmptyTile(map);
}

void getPlayerSpawnPos(Game *pGame)
{
    SDL_Rect spawnTile = findSpawnTile(pGame->pMap); // this function returns a valid spawn tile

    pGame->pPlayers[0].x = spawnTile.x;
    pGame->pPlayers[0].y = spawnTile.y;
    pGame->pPlayers[0].rect.x = spawnTile.x;
    pGame->pPlayers[0].rect.y = spawnTile.y;
}

void translatePositionToScreen(Game *pGame)
{
    float scaleY = (float)pGame->pMap[0].rect.h / pGame->tileSize;
    float scaleX = (float)pGame->pMap[0].rect.w / pGame->tileSize;
    int offsetX = pGame->pMap[0].rect.x - pGame->pMap[0].x;
    int offsetY = pGame->pMap[0].rect.y - pGame->pMap[0].y;
    for (int i = 0; i < 1; i++)
    {
        pGame->pPlayers[i].rect.x = ((float)pGame->pPlayers[i].x * scaleX) + offsetX;
        pGame->pPlayers[i].rect.y = ((float)pGame->pPlayers[i].y * scaleY) + offsetY;

        pGame->pPlayers[i].rect.h = pGame->tileSize;//(pGame->tileSize / 2) + ((pGame->tileSize / 2) * (1 - (float)pGame->pPlayers[i].charge / MAX_CHARGE));
        pGame->pPlayers[i].rect.y += pGame->tileSize - pGame->pPlayers[i].rect.h;
        pGame->pPlayers[i].rect.w = pGame->tileSize;
    }
}