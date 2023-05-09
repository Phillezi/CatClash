#include "definitions.h"
#include "player.h"
#include "levelEditor.h"
#include "text.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

void centerPlayer(Game *pGame, Player *pPlayer)
{
    while (pGame->isDrawing)
        ; // temporary fix to screenTearing?

    int screenShiftAmount = pGame->movementAmount;

    bool playerIsInOneFifthOfScreenBorder       = (pPlayer->rect.x >= (4 * pGame->windowWidth) / 5 || pPlayer->rect.x <= pGame->windowWidth / 5) || (pPlayer->rect.y >= (4 * pGame->windowHeight) / 5 || pPlayer->rect.y <= pGame->windowHeight / 5);
    bool playerIsOutsideScreen                  = (pPlayer->rect.x >= pGame->windowWidth || pPlayer->rect.x <= 0) || (pPlayer->rect.y >= pGame->windowHeight || pPlayer->rect.y <= 0);
    bool playerIsMoreThanOneTileOutsideOfScreen = (pPlayer->rect.x >= pGame->windowWidth + pGame->world.tileSize || pPlayer->rect.x <= -pGame->world.tileSize) || (pPlayer->rect.y >= pGame->windowHeight + pGame->world.tileSize || pPlayer->rect.y <= -pGame->world.tileSize);
    bool playerIsCloseToUpperBorder             = pPlayer->rect.y < (2 * pGame->windowHeight) / 5;
    bool playerIsCloseToLowerBorder             = pPlayer->rect.y > (3 * pGame->windowHeight) / 5;
    bool playerIsCloseToLeftBorder              = pPlayer->rect.x < (2 * pGame->windowWidth) / 5;
    bool playerIsCloseToRigthBorder             = pPlayer->rect.x > (3 * pGame->windowWidth) / 5;

    if (playerIsMoreThanOneTileOutsideOfScreen)
        screenShiftAmount = pGame->movementAmount * 30;
    else if (playerIsOutsideScreen)
        screenShiftAmount = pGame->movementAmount * 20;
    else if (playerIsInOneFifthOfScreenBorder)
        screenShiftAmount = pGame->movementAmount * 2;

    if (playerIsCloseToUpperBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y += screenShiftAmount;
        }
    }
    if (playerIsCloseToLowerBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y -= screenShiftAmount;
        }
    }
    if (playerIsCloseToLeftBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x += screenShiftAmount;
        }
    }
    if (playerIsCloseToRigthBorder)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x -= screenShiftAmount;
        }
    }
}

void *handleInput(void *pGameIn) // Game *pGame)
{
    Game *pGame = (Game *)pGameIn;
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    float scaleY = (float)pGame->map[0].wall.h / pGame->world.tileSize;
    float scaleX = (float)pGame->map[0].wall.w / pGame->world.tileSize;
    if (pGame->pPlayer->charge == 0)
        pGame->pPlayer->charging = 0;

    if (currentKeyStates[SDL_SCANCODE_Q])
    {
        if (pGame->world.angle >= 0.01)
        {
            pGame->world.angle -= 0.01;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                pGame->map[i].wall.h = ((float)pGame->world.tileSize * (1 - pGame->world.angle));

                if (i > (MAPSIZE - 1))
                {
                    pGame->map[i].wall.y = pGame->map[i - MAPSIZE].wall.y + pGame->map[i].wall.h;
                }
            }
        }
    }
    if (currentKeyStates[SDL_SCANCODE_E])
    {
        if (pGame->world.angle < 1)
        {
            pGame->world.angle += 0.01;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                pGame->map[i].wall.h = ((float)pGame->world.tileSize * (1 - pGame->world.angle));

                if (i > (MAPSIZE - 1))
                {
                    pGame->map[i].wall.y = pGame->map[i - MAPSIZE].wall.y + pGame->map[i].wall.h;
                }
            }
        }
    }
    if (currentKeyStates[SDL_SCANCODE_SPACE] && pGame->pPlayer->charging == 0)
    {
        pGame->pPlayer->idle = 1;
        pGame->state = START;
        if (pGame->pPlayer->charge < MAX_CHARGE)
        {
            pGame->pPlayer->charge += 1;
        }
        else
        {
            printf("FULLY CHARGED\n");
        }
    }
    else if (pGame->pPlayer->charge > 0)
    {
        srand(time(NULL));
        int damage = 0, id = 0;
        static int flag = 0;
        pGame->pPlayer->charging = 1;

        for (int i = 0; i < 2 * (pGame->pPlayer->charge / 2); i++)
        {
            if (checkCollision(*pGame->pPlayer, pGame->map, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize) == 1)
            {
                int temp = rand() % pGame->nrOfPortals;
                pGame->pPlayer->x = pGame->portalList[temp].x;
                pGame->pPlayer->y = pGame->portalList[temp].y;
            }
            if (((id = playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize, 0)) == -1) && (checkCollision(*pGame->pPlayer, pGame->map, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize) <= 0))
            {
                movePlayer(pGame->pPlayer, pGame->pPlayer->prevKeyPressed);
            }
            else
            {
                // damage = pGame->pPlayer->charge * 2;    // Collided with wall
                if (id != -1)
                {
                    if (flag)
                    {
                        pGame->pPlayer->charge = 1;
                        flag = 0;
                    }
                    else
                        flag = 1;
                }
                else
                {
                    damage = pGame->pPlayer->charge * 2;
                    pGame->pPlayer->charge = 1;
                }
                break;
            }
        }
        pGame->pPlayer->hp -= damage;
        pGame->pPlayer->charge -= 1;
    }
    else
    {
        srand(time(NULL));
        for (int i = 0; i < pGame->movementAmount; i++)
        {
            pGame->pPlayer->idle = 1;
            if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
            {
                if (checkCollision(*pGame->pPlayer, pGame->map, 'W', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    while ((pGame->portalList[temp].y / pGame->world.tileSize) - (pGame->pPlayer->y / pGame->world.tileSize) >= -1 && (pGame->portalList[temp].y / pGame->world.tileSize) - (pGame->pPlayer->y / pGame->world.tileSize) <= 1)
                    {
                        int temp = rand() % pGame->nrOfPortals;
                        pGame->pPlayer->x = pGame->portalList[temp].x;
                        pGame->pPlayer->y = pGame->portalList[temp].y;
                    }
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'W', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'W', pGame->world.tileSize, 0) == -1))
                {
                    movePlayer(pGame->pPlayer, 'W');
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
            {
                if (checkCollision(*pGame->pPlayer, pGame->map, 'A', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    while ((pGame->portalList[temp].x / pGame->world.tileSize) - (pGame->pPlayer->x / pGame->world.tileSize) >= -1 && (pGame->portalList[temp].x / pGame->world.tileSize) - (pGame->pPlayer->x / pGame->world.tileSize) <= 1)
                    {
                        int temp = rand() % pGame->nrOfPortals;
                        pGame->pPlayer->x = pGame->portalList[temp].x;
                        pGame->pPlayer->y = pGame->portalList[temp].y;
                    }
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'A', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'A', pGame->world.tileSize, 0) == -1))
                {
                    movePlayer(pGame->pPlayer, 'A');
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
            {
                if (checkCollision(*pGame->pPlayer, pGame->map, 'S', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    while ((pGame->portalList[temp].y / pGame->world.tileSize) - (pGame->pPlayer->y / pGame->world.tileSize) >= -1 && (pGame->portalList[temp].y / pGame->world.tileSize) - (pGame->pPlayer->y / pGame->world.tileSize) <= 1)
                    {
                        int temp = rand() % pGame->nrOfPortals;
                        pGame->pPlayer->x = pGame->portalList[temp].x;
                        pGame->pPlayer->y = pGame->portalList[temp].y;
                    }
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'S', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'S', pGame->world.tileSize, 0) == -1))
                {
                    movePlayer(pGame->pPlayer, 'S');
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
            {
                if (checkCollision(*pGame->pPlayer, pGame->map, 'D', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    while ((pGame->portalList[temp].x / pGame->world.tileSize) - (pGame->pPlayer->x / pGame->world.tileSize) >= -1 && (pGame->portalList[temp].x / pGame->world.tileSize) - (pGame->pPlayer->x / pGame->world.tileSize) <= 1)
                    {
                        int temp = rand() % pGame->nrOfPortals;
                        pGame->pPlayer->x = pGame->portalList[temp].x;
                        pGame->pPlayer->y = pGame->portalList[temp].y;
                    }
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'D', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'D', pGame->world.tileSize, 0) == -1))
                {
                    movePlayer(pGame->pPlayer, 'D');
                }
            }
        }
    }

    int offsetX = pGame->map[0].wall.x - pGame->map[0].x;
    int offsetY = pGame->map[0].wall.y - pGame->map[0].y;
    pGame->pPlayer->rect.x = ((float)pGame->pPlayer->x * scaleX) + offsetX;
    pGame->pPlayer->rect.y = ((float)pGame->pPlayer->y * scaleY) + offsetY;

    pGame->pPlayer->rect.h = (pGame->world.tileSize / 2) + ((pGame->world.tileSize / 2) * (1 - (float)pGame->pPlayer->charge / MAX_CHARGE));
    pGame->pPlayer->rect.y += pGame->world.tileSize - pGame->pPlayer->rect.h;

    // CENTER PLAYER + SPECTATE

    switch (pGame->pPlayer->state)
    {
    case ALIVE:
        centerPlayer(pGame, pGame->pPlayer);
        break;
    case DEAD:
        if (&pGame->pMultiPlayer[pGame->tempID])
            centerPlayer(pGame, &pGame->pMultiPlayer[pGame->tempID]);
        break;
    }
    return 0;
}

void movePlayer(Player *pPlayer, char direction)
{
    if (pPlayer->state == ALIVE)
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
    pPlayer->prevKeyPressed = direction;
    pPlayer->idle = 0;
}

int playerCollision(Player player, Player players[], int nrOfPlayers, char direction, int tileSize, int extraLength)
{
    if (nrOfPlayers == 0)
        return -1;
    for (int i = 0; i < nrOfPlayers; i++)
    {
        if (players[i].state == DEAD) continue;
        switch (direction)
        {
        case 'W': // First checks if rows overlap then if columns overlap
            if ((player.y > players[i].y) && (player.y - 1 - extraLength < players[i].y + tileSize - 1))
                if (((player.x == players[i].x) || ((players[i].x + (tileSize - 1) > player.x) && (players[i].x < player.x)) || ((players[i].x > player.x) && (players[i].x < player.x + (tileSize - 1)))))
                    return i;
            break;
        case 'A': // First checks if columns overlap then if rows overlap
            if ((player.x > players[i].x) && (player.x - 1 - extraLength < players[i].x + tileSize - 1))
                if (((player.y == players[i].y) || ((players[i].y + (tileSize - 1) > player.y) && (players[i].y < player.y)) || ((players[i].y > player.y) && (players[i].y < player.y + (tileSize - 1)))))
                    return i;
            break;
        case 'S': // First checks if rows overlap then if columns overlap
            if ((player.y < players[i].y) && (player.y + tileSize + extraLength > players[i].y))
                if (((player.x == players[i].x) || ((players[i].x + (tileSize - 1) > player.x) && (players[i].x < player.x)) || ((players[i].x > player.x) && (players[i].x < player.x + (tileSize - 1)))))
                    return i;
            break;
        case 'D': // First checks if columns overlap then if rows overlap
            if ((player.x < players[i].x) && (player.x + tileSize + extraLength > players[i].x))
                if (((player.y == players[i].y) || ((players[i].y + (tileSize - 1) > player.y) && (players[i].y < player.y)) || ((players[i].y > player.y) && (players[i].y < player.y + (tileSize - 1)))))
                    return i;
            break;
        }
    }
    return -1;
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
            return map[i].wall;
        }
    }
}

SDL_Rect findSpawnTile(Tile map[])
{
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (map[i].type == -1)
        {
            return map[i].wall;
        }
    }
    printf("Error: No spawn Tile in Selected map!\n");
    return findEmptyTile(map);
}

void getPlayerSpawnPos(Game *pGame)
{
    SDL_Rect spawnTile = findSpawnTile(pGame->map); // this function returns a valid spawn tile

    // for adt
    pGame->pPlayer->x = spawnTile.x;
    pGame->pPlayer->y = spawnTile.y;
    pGame->pPlayer->rect.x = spawnTile.x;
    pGame->pPlayer->rect.y = spawnTile.y;
}

Player *createPlayer(int id, char *name, int tileSize)
{
    Player *pPlayer = (Player *)malloc(sizeof(Player));
    if (!pPlayer)
    {
        printf("Error: Failed to allocate memory for player\n");
        return NULL;
    }

    pPlayer->charge = 0;
    pPlayer->hp = 255;
    pPlayer->id = id;
    if (strlen(name) < MAX_NAME_LEN)
        strcpy(pPlayer->name, name);
    else
        strcpy(pPlayer->name, "ERROR");
    pPlayer->prevKeyPressed = 'D';
    pPlayer->rect.w = tileSize;
    pPlayer->rect.h = tileSize;
    pPlayer->rect.x = 0;
    pPlayer->rect.y = 0;
    pPlayer->x = 0;
    pPlayer->y = 0;
    pPlayer->state = ALIVE;

    return pPlayer;
}

void destroyPlayer(Player *pPlayer)
{
    free(pPlayer);
}

void translatePositionToScreen(Game *pGame)
{
    float scaleY = (float)pGame->map[0].wall.h / pGame->world.tileSize;
    float scaleX = (float)pGame->map[0].wall.w / pGame->world.tileSize;
    int offsetX = pGame->map[0].wall.x - pGame->map[0].x;
    int offsetY = pGame->map[0].wall.y - pGame->map[0].y;
    for (int i = 0; i < pGame->nrOfPlayers; i++)
    {
        pGame->pMultiPlayer[i].rect.x = ((float)pGame->pMultiPlayer[i].x * scaleX) + offsetX;
        pGame->pMultiPlayer[i].rect.y = ((float)pGame->pMultiPlayer[i].y * scaleY) + offsetY;

        pGame->pMultiPlayer[i].rect.h = (pGame->world.tileSize / 2) + ((pGame->world.tileSize / 2) * (1 - (float)pGame->pMultiPlayer[i].charge / MAX_CHARGE));
        pGame->pMultiPlayer[i].rect.y += pGame->world.tileSize - pGame->pMultiPlayer[i].rect.h;
        pGame->pMultiPlayer[i].rect.w = pGame->world.tileSize;
    }
}

int changePlayerTexture(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer, char direction)
{
    static bool textureLoaded = false;

    SDL_Rect srcRect;
    srcRect.x = 611; // test img X starting point
    srcRect.y = 485; // test img Y starting point
    srcRect.w = 24;
    srcRect.h = 24;

    switch (direction)
    {
    case 'W':
        srcRect.x = 516;
        srcRect.y = 320;
        break;
    case 'S':
        srcRect.x = 612;
        srcRect.y = 33;
        break;
    case 'D':
        srcRect.x = 611;
        srcRect.y = 480;
        break;
    case 'A':
        srcRect.x = 613;
        srcRect.y = 96;
        break;
    default:
        break;
    }

    static SDL_Surface *pSurface = NULL; // static pointer to SDL_Surface to hold the texture
    if (!textureLoaded)
    {
        pSurface = IMG_Load("resources/cat3.png");
        if (!pSurface)
        {
            return -1;
        }
        textureLoaded = true; // set the flag to indicate that the texture has been loaded
    }

    SDL_Surface *pCroppedSurface = SDL_CreateRGBSurface(0, 24, 24, pSurface->format->BitsPerPixel,
                                                        pSurface->format->Rmask, pSurface->format->Gmask,
                                                        pSurface->format->Bmask, pSurface->format->Amask);

    SDL_BlitSurface(pSurface, &srcRect, pCroppedSurface, NULL);

    *pTexturePlayer = SDL_CreateTextureFromSurface(pRenderer, pCroppedSurface);
    SDL_FreeSurface(pCroppedSurface);

    if (!pTexturePlayer)
    {
        return -1;
    }
    return 0;
}

void loadMedia(SDL_Renderer *pRenderer, SDL_Texture **pPlayerTexture, SDL_Rect gSpriteClips[][29], int playerID)
{
    static bool textureLoaded = false;

    if (!textureLoaded)
    {
        SDL_Surface *gCatSurface = IMG_Load("resources/cat3.PNG");
        *pPlayerTexture = SDL_CreateTextureFromSurface(pRenderer, gCatSurface);
        textureLoaded = true;
    }

    int w = 24, h = 24;
    // playerID--;   //for testing

    // IDLE
    gSpriteClips[playerID][0].x = 36;
    gSpriteClips[playerID][0].y = 34 + (playerID * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[playerID][0].w = 24;
    gSpriteClips[playerID][0].h = 24;

    gSpriteClips[playerID][1].x = 36;
    gSpriteClips[playerID][1].y = 290 + (playerID * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[playerID][1].w = 24;
    gSpriteClips[playerID][1].h = 24;

    gSpriteClips[playerID][2].x = 5;
    gSpriteClips[playerID][2].y = 482 + (playerID * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[playerID][2].w = 24;
    gSpriteClips[playerID][2].h = 24;

    // DEAD | WIN
    gSpriteClips[playerID][27].x = 195;
    gSpriteClips[playerID][27].y = 71 + (playerID * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[playerID][27].w = 24;
    gSpriteClips[playerID][27].h = 24;

    gSpriteClips[playerID][28].x = 228;
    gSpriteClips[playerID][28].y = 71 + (playerID * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[playerID][28].w = 24;
    gSpriteClips[playerID][28].h = 24;

    // RUNNING DOWN
    int x[4] = {644, 676, 708, 740};
    int y[2] = {32 + (playerID * HEIGHT_OF_PLAYER_SPRITE), 63 + (playerID * HEIGHT_OF_PLAYER_SPRITE)};

    gSpriteClips[playerID][3].x = x[0];
    gSpriteClips[playerID][3].y = y[1];
    gSpriteClips[playerID][3].w = w;
    gSpriteClips[playerID][3].h = h;

    gSpriteClips[playerID][4].x = x[1];
    gSpriteClips[playerID][4].y = y[1];
    gSpriteClips[playerID][4].w = w;
    gSpriteClips[playerID][4].h = h;

    gSpriteClips[playerID][5].x = x[2];
    gSpriteClips[playerID][5].y = y[1];
    gSpriteClips[playerID][5].w = w;
    gSpriteClips[playerID][5].h = h;

    gSpriteClips[playerID][6].x = x[3];
    gSpriteClips[playerID][6].y = y[1];
    gSpriteClips[playerID][6].w = w;
    gSpriteClips[playerID][6].h = h;

    gSpriteClips[playerID][7].x = x[0];
    gSpriteClips[playerID][7].y = y[0];
    gSpriteClips[playerID][7].w = w;
    gSpriteClips[playerID][7].h = h;

    gSpriteClips[playerID][8].x = x[1];
    gSpriteClips[playerID][8].y = y[0];
    gSpriteClips[playerID][8].w = w;
    gSpriteClips[playerID][8].h = h;

    gSpriteClips[playerID][9].x = x[2];
    gSpriteClips[playerID][9].y = y[0];
    gSpriteClips[playerID][9].w = w;
    gSpriteClips[playerID][9].h = h;

    gSpriteClips[playerID][10].x = x[3];
    gSpriteClips[playerID][10].y = y[0];
    gSpriteClips[playerID][10].w = w;
    gSpriteClips[playerID][10].h = h;

    // RUNNING UP
    y[0] = 289 + (playerID * HEIGHT_OF_PLAYER_SPRITE), y[1] = 321 + (playerID * HEIGHT_OF_PLAYER_SPRITE);

    gSpriteClips[playerID][11].x = x[0];
    gSpriteClips[playerID][11].y = y[1];
    gSpriteClips[playerID][11].w = w;
    gSpriteClips[playerID][11].h = h;

    gSpriteClips[playerID][12].x = x[1];
    gSpriteClips[playerID][12].y = y[1];
    gSpriteClips[playerID][12].w = w;
    gSpriteClips[playerID][12].h = h;

    gSpriteClips[playerID][13].x = x[2];
    gSpriteClips[playerID][13].y = y[1];
    gSpriteClips[playerID][13].w = w;
    gSpriteClips[playerID][13].h = h;

    gSpriteClips[playerID][14].x = x[3];
    gSpriteClips[playerID][14].y = y[1];
    gSpriteClips[playerID][14].w = w;
    gSpriteClips[playerID][14].h = h;

    gSpriteClips[playerID][15].x = x[0];
    gSpriteClips[playerID][15].y = y[0];
    gSpriteClips[playerID][15].w = w;
    gSpriteClips[playerID][15].h = h;

    gSpriteClips[playerID][16].x = x[1];
    gSpriteClips[playerID][16].y = y[0];
    gSpriteClips[playerID][16].w = w;
    gSpriteClips[playerID][16].h = h;

    gSpriteClips[playerID][17].x = x[2];
    gSpriteClips[playerID][17].y = y[0];
    gSpriteClips[playerID][17].w = w;
    gSpriteClips[playerID][17].h = h;

    gSpriteClips[playerID][18].x = x[3];
    gSpriteClips[playerID][18].y = y[0];
    gSpriteClips[playerID][18].w = w;
    gSpriteClips[playerID][18].h = h;

    // RUNNING LEFT/RIGHT
    x[0] = 642, x[1] = 674, x[2] = 706, x[3] = 738;
    y[0] = 482 + (playerID * HEIGHT_OF_PLAYER_SPRITE), y[1] = 512 + (playerID * HEIGHT_OF_PLAYER_SPRITE);

    gSpriteClips[playerID][19].x = x[0];
    gSpriteClips[playerID][19].y = y[1];
    gSpriteClips[playerID][19].w = w;
    gSpriteClips[playerID][19].h = h;

    gSpriteClips[playerID][20].x = x[1];
    gSpriteClips[playerID][20].y = y[1];
    gSpriteClips[playerID][20].w = w;
    gSpriteClips[playerID][20].h = h;

    gSpriteClips[playerID][21].x = x[2];
    gSpriteClips[playerID][21].y = y[1];
    gSpriteClips[playerID][21].w = w;
    gSpriteClips[playerID][21].h = h;

    gSpriteClips[playerID][22].x = x[3];
    gSpriteClips[playerID][22].y = y[1];
    gSpriteClips[playerID][22].w = w;
    gSpriteClips[playerID][22].h = h;

    gSpriteClips[playerID][23].x = x[0];
    gSpriteClips[playerID][23].y = y[0];
    gSpriteClips[playerID][23].w = w;
    gSpriteClips[playerID][23].h = h;

    gSpriteClips[playerID][24].x = x[1];
    gSpriteClips[playerID][24].y = y[0];
    gSpriteClips[playerID][24].w = w;
    gSpriteClips[playerID][24].h = h;

    gSpriteClips[playerID][25].x = x[2];
    gSpriteClips[playerID][25].y = y[0];
    gSpriteClips[playerID][25].w = w;
    gSpriteClips[playerID][25].h = h;

    gSpriteClips[playerID][26].x = x[3];
    gSpriteClips[playerID][26].y = y[0];
    gSpriteClips[playerID][26].w = w;
    gSpriteClips[playerID][26].h = h;
}

void drawPlayer(Game *pGame, Player player, int i)
{
    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(pGame->pRenderer, &player.rect);
    SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;

    static int frame[MAX_PLAYERS] = {0};
    static int prevTime[MAX_PLAYERS] = {0};

    if (player.state == ALIVE)
    {
        switch (player.prevKeyPressed)
        {
        case 'W':
            if (player.idle)
            {
                frame[i] = 0;
                prevTime[i] = SDL_GetTicks();
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][1], &player.rect, 0, NULL, SDL_FLIP_NONE);
            }
            else
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][frame[i] + 3 + 8], &player.rect, 0, NULL, SDL_FLIP_NONE);
            break;
        case 'S':
            if (player.idle)
            {
                frame[i] = 0;
                prevTime[i] = SDL_GetTicks();
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][0], &player.rect, 0, NULL, SDL_FLIP_NONE);
            }
            else
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][frame[i] + 3], &player.rect, 0, NULL, SDL_FLIP_NONE);
            break;
        case 'D':
            if (player.idle)
            {
                frame[i] = 0;
                prevTime[i] = SDL_GetTicks();
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][2], &player.rect, 0, NULL, SDL_FLIP_NONE);
            }
            else
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][frame[i] + 3 + 8 + 8], &player.rect, 0, NULL, SDL_FLIP_NONE);
            break;
        case 'A':
            if (player.idle)
            {
                frame[i] = 0;
                prevTime[i] = SDL_GetTicks();
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][2], &player.rect, 0, NULL, flip);
            }
            else
                SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][frame[i] + 3 + 8 + 8], &player.rect, 0, NULL, flip);
            break;
        }
        if (((SDL_GetTicks() - prevTime[i]) % 1000) / 95) { frame[i]++; prevTime[i] = SDL_GetTicks(); }
        if (frame[i] >= 8) frame[i] = 0;
    }
    else if (player.state == DEAD)
        SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][27], &player.rect, 0, NULL, SDL_FLIP_NONE);
    else if (player.state == WIN)
        SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[i][28], &player.rect, 0, NULL, SDL_FLIP_NONE);
}

Player *createNewMultiPlayer(Game *pGame, int size, Player data)
{
    Player *pNew_arr;
    if (pGame->pMultiPlayer)
    {
        printf("Expanding memory for player array\n");
        pNew_arr = (Player *)realloc(pGame->pMultiPlayer, (size + 1) * sizeof(Player));
    }
    else
    {
        pNew_arr = createPlayer(data.id, "alloc", pGame->world.tileSize);
    }

    if (pNew_arr == NULL)
    {
        printf("ERROR when allocating new memory for joined player\n");
        return NULL;
    }

    pNew_arr[size] = data;

    return pNew_arr;
}

Player *removePlayer(Game *pGame, int size)
{
    if (pGame->pMultiPlayer == NULL)
    {
        printf("ERROR: Player array is empty\n");
        return NULL;
    }

    printf("Shrinking memory for player array\n");
    Player *pNew_arr = (Player *)realloc(pGame->pMultiPlayer, size * sizeof(Player));
    if (pNew_arr == NULL && size > 0)
    {
        printf("ERROR when reallocating memory for player array\n");
        return NULL;
    }

    pGame->pMultiPlayer = pNew_arr;
    return pNew_arr;
}

int getAlivePlayers(Game *pGame)
{
    int nrOfPlayersAlive = 0;
    for (int i = 0; i < pGame->nrOfPlayers; i++)
    {
        if (pGame->pMultiPlayer[i].state == ALIVE)
        {
            nrOfPlayersAlive++;
        }
    }
    return nrOfPlayersAlive;
}

int getDeadPlayers(Game *pGame)
{
    int deadPlayers = 0;
    for (int i = 0; i < pGame->nrOfPlayers; i++)
    {
        if (pGame->pMultiPlayer[i].state == DEAD)
        {
            deadPlayers++;
        }
    }
    return deadPlayers;
}

void checkChargingPlayers(Game *pGame) {
    // Checking if other players charge into you
    int id = 0, collision = 0, extraLength = 0;
    static int invincibilityTicks = 0, prevTime = 0;
    static char dir[4] = {'W', 'A', 'S', 'D'};
    
    if ((SDL_GetTicks() - prevTime) % 2000  >= invincibilityTicks)
    {
        invincibilityTicks = 0;
        prevTime = 0;

        for (int i = 0; i < 4; i++){
            extraLength = 0;
            if      (dir[i] == 'W' && pGame->pPlayer->prevKeyPressed == 'S') extraLength = 10;
            else if (dir[i] == 'A' && pGame->pPlayer->prevKeyPressed == 'D') extraLength = 10;
            else if (dir[i] == 'S' && pGame->pPlayer->prevKeyPressed == 'W') extraLength = 10;
            else if (dir[i] == 'D' && pGame->pPlayer->prevKeyPressed == 'A') extraLength = 10;

            if ((id = playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, dir[i], pGame->world.tileSize, extraLength)) != -1)
                if (pGame->pMultiPlayer[id].charging) { 
                    int oldHealth = pGame->pPlayer->hp;
                    damagePlayer(pGame, id, dir[i]);

                    if (oldHealth > pGame->pPlayer->hp) {
                        prevTime = SDL_GetTicks();
                        invincibilityTicks = 500;
                    }
                    break; 
                }
        }
    }
}

void damagePlayer(Game *pGame, int id, char direction) {
    int tmp;
    printf("player charge: %d\tcolliding charge: %d\n", pGame->pPlayer->charge, pGame->pMultiPlayer[id].charge);

    if (pGame->pPlayer->prevKeyPressed != direction) {
        printf("You take damage in func 1\n");
        pGame->pPlayer->hp -= pGame->pMultiPlayer[id].charge * 2;
    }
    else if (pGame->pPlayer->charge < pGame->pMultiPlayer[id].charge && headOnCollision(pGame, id) == -1) {
        printf("You take damage in func 2\n");
        pGame->pPlayer->hp -= (pGame->pMultiPlayer[id].charge - pGame->pPlayer->charge) * 2;
    }
    else if (pGame->pPlayer->charging == 0 && chargingIntoMe(pGame, id, direction)){
        printf("You take damage in func 3\n");
        pGame->pPlayer->hp -= pGame->pMultiPlayer[id].charge * 2;
    }
}

/* \returns 1 if players are not in a head on collision, -1 if they are */
int headOnCollision(Game *pGame, int id) {
    switch (pGame->pPlayer->prevKeyPressed) {
    case 'W': if (pGame->pMultiPlayer[id].prevKeyPressed != 'S') return 1; break;
    case 'A': if (pGame->pMultiPlayer[id].prevKeyPressed != 'D') return 1; break;
    case 'S': if (pGame->pMultiPlayer[id].prevKeyPressed != 'W') return 1; break;
    case 'D': if (pGame->pMultiPlayer[id].prevKeyPressed != 'A') return 1; break;
    }
    return -1;
}

/* \returns 1 if opposing player is charging into you, otherwise 0 */
int chargingIntoMe(Game *pGame, int id, char direction) {
    switch (direction) {
    case 'W': if (pGame->pMultiPlayer[id].prevKeyPressed == 'S') return 1; break;
    case 'A': if (pGame->pMultiPlayer[id].prevKeyPressed == 'D') return 1; break;
    case 'S': if (pGame->pMultiPlayer[id].prevKeyPressed == 'W') return 1; break;
    case 'D': if (pGame->pMultiPlayer[id].prevKeyPressed == 'A') return 1; break;
    }
    return 0;
}