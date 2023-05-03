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
    int screenShiftAmount = pGame->movementAmount;
    if (pPlayer->rect.x >= (4 * pGame->windowWidth) / 5 || pPlayer->rect.x <= pGame->windowWidth / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pPlayer->rect.y >= (4 * pGame->windowHeight) / 5 || pPlayer->rect.y <= pGame->windowHeight / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pPlayer->rect.x >= pGame->windowWidth || pPlayer->rect.x <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pPlayer->rect.y >= pGame->windowHeight || pPlayer->rect.y <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }

    if (pPlayer->rect.x >= pGame->windowWidth + pGame->world.tileSize || pPlayer->rect.x <= -pGame->world.tileSize)
    {
        screenShiftAmount = pGame->movementAmount * 30;
    }
    if (pPlayer->rect.y >= pGame->windowHeight + pGame->world.tileSize || pPlayer->rect.y <= -pGame->world.tileSize)
    {
        screenShiftAmount = pGame->movementAmount *  30;
    }

    if (pPlayer->rect.y < (2 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y += screenShiftAmount;
        }
    }
    if (pPlayer->rect.y > (3 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y -= screenShiftAmount;
        }
    }
    if (pPlayer->rect.x < (2 * pGame->windowWidth) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x += screenShiftAmount;
        }
    }
    if (pPlayer->rect.x > (3 * pGame->windowWidth) / 5)
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
    if (currentKeyStates[SDL_SCANCODE_SPACE])
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
        int damage = 0;
        int id = 0;

        for (int i = 0; i < 2 * (pGame->pPlayer->charge / 2); i++)
        {
            if (checkCollision(*pGame->pPlayer, pGame->map, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize) == 1) {
                int temp = rand() % pGame->nrOfPortals;
                pGame->pPlayer->x = pGame->portalList[temp].x;
                pGame->pPlayer->y = pGame->portalList[temp].y;
            }
            if (((id = playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize)) == -1) && (checkCollision(*pGame->pPlayer, pGame->map, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize) <= 0))
            {
                movePlayer(pGame->pPlayer, pGame->pPlayer->prevKeyPressed);
                pGame->pPlayer->idle = 0;
            }
            else
            {
                if (id != -1) {
                    if (pGame->pPlayer->charge < pGame->pMultiPlayer[id].charge){
                        damage = pGame->pMultiPlayer->charge * 2;    // Collided with player
                        printf("Your charge: %d\nCharge of player you collided with: %d\n", pGame->pPlayer->charge, pGame->pMultiPlayer[id].charge);
                    }
                    else damage = 0;                             // Collided with player but you have higher charge
                }
                else damage = pGame->pPlayer->charge * 2;    // Collided with wall
                pGame->pPlayer->charge = 1;
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
                    pGame->pPlayer->x = pGame->portalList[temp].x;
                    pGame->pPlayer->y = pGame->portalList[temp].y;
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'W', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'W', pGame->world.tileSize) == -1))
                {
                    pGame->pPlayer->prevKeyPressed = 'W';
                    movePlayer(pGame->pPlayer, 'W');
                    pGame->pPlayer->idle = 0;
                }
                else
                {
                    // printf("COLLISION W\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
            {

                if (checkCollision(*pGame->pPlayer, pGame->map, 'A', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    pGame->pPlayer->x = pGame->portalList[temp].x;
                    pGame->pPlayer->y = pGame->portalList[temp].y;
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'A', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'A', pGame->world.tileSize) == -1))
                {
                    pGame->pPlayer->prevKeyPressed = 'A';
                    movePlayer(pGame->pPlayer, 'A');
                    pGame->pPlayer->idle = 0;
                }
                else
                {
                    // printf("COLLISION A\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
            {
                if (checkCollision(*pGame->pPlayer, pGame->map, 'S', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    pGame->pPlayer->x = pGame->portalList[temp].x;
                    pGame->pPlayer->y = pGame->portalList[temp].y;
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'S', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'S', pGame->world.tileSize) == -1))
                {
                    pGame->pPlayer->prevKeyPressed = 'S';
                    movePlayer(pGame->pPlayer, 'S');
                    pGame->pPlayer->idle = 0;
                }
                else
                {
                    // printf("COLLISION S\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
            {
                if (checkCollision(*pGame->pPlayer, pGame->map, 'D', pGame->world.tileSize) == 1)
                {
                    int temp = rand() % pGame->nrOfPortals;
                    pGame->pPlayer->x = pGame->portalList[temp].x;
                    pGame->pPlayer->y = pGame->portalList[temp].y;
                }
                if ((checkCollision(*pGame->pPlayer, pGame->map, 'D', pGame->world.tileSize) < 1) && (playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, 'D', pGame->world.tileSize) == -1))
                {
                    pGame->pPlayer->prevKeyPressed = 'D';
                    movePlayer(pGame->pPlayer, 'D');
                    pGame->pPlayer->idle = 0;
                }
                else
                {
                    // printf("COLLISION D\n");
                }
            }

            /*
            if (!checkCollision(*pPlayer, map, *pPrevKeypressed))
                {
                    movePlayer(pPlayer, *pPrevKeypressed);
                }
                else
                {
                    printf("COLLISION %c\n", *pPrevKeypressed);
                }
                */
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
}

int playerCollision(Player player, Player players[], int nrOfPlayers, char direction, int tileSize) {
    if (nrOfPlayers == 0) return -1;
    for (int i = 0; i < nrOfPlayers; i++) {
        switch (direction) {
        case 'W':   // First checks if rows overlap then if columns overlap
            if ((player.y > players[i].y) && (player.y-1 < players[i].y+tileSize-1)) 
                if (((player.x == players[i].x) || ((players[i].x + (tileSize-1) > player.x) && (players[i].x < player.x)) || ((players[i].x > player.x) && (players[i].x < player.x + (tileSize-1))))) return i;
            return -1;
            break;
        case 'A':   // First checks if columns overlap then if rows overlap
            if ((player.x > players[i].x) && (player.x-1 < players[i].x+tileSize-1))
                if (((player.y == players[i].y) || ((players[i].y + (tileSize-1) > player.y) && (players[i].y < player.y)) || ((players[i].y > player.y) && (players[i].y < player.y + (tileSize-1))))) return i;
            return -1;
            break;
        case 'S':   // First checks if rows overlap then if columns overlap
            if ((player.y < players[i].y) && (player.y + tileSize > players[i].y))
                if (((player.x == players[i].x) || ((players[i].x + (tileSize-1) > player.x) && (players[i].x < player.x)) || ((players[i].x > player.x) && (players[i].x < player.x + (tileSize-1))))) return i;
            return -1;
            break;
        case 'D':   // First checks if columns overlap then if rows overlap
            if ((player.x < players[i].x) && (player.x + tileSize > players[i].x)) 
                if (((player.y == players[i].y) || ((players[i].y + (tileSize-1) > player.y) && (players[i].y < player.y)) || ((players[i].y > player.y) && (players[i].y < player.y + (tileSize-1))))) return i;
            return -1;
            break;
        }
    }
}

int checkCollision(Player player, Tile map[], char direction, int tileSize)
{
    switch (direction)
    {
    case 'W':
        if (map[(((player.y - 1) / tileSize) * MAPSIZE) + (player.x / tileSize)].type)
            return map[(((player.y - 1) / tileSize) * MAPSIZE) + (player.x / tileSize)].type;
        else if (map[(((player.y - 1) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type)
            return map[(((player.y - 1) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type;
        break;
    case 'A':
        if (map[(((player.y) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type)
            return map[(((player.y) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type;
        else if (map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type)
            return map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type;
        break;
    case 'S':
        if (map[(((player.y + tileSize) / tileSize) * MAPSIZE) + (player.x / tileSize)].type)
            return map[(((player.y + tileSize) / tileSize) * MAPSIZE) + (player.x / tileSize)].type;
        else if (map[(((player.y + tileSize) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type)
            return map[(((player.y + tileSize) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type;
        break;
    case 'D':
        if (map[(((player.y) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type)
            return map[(((player.y) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type;
        else if (map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type)
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
    /*
    pGame->player.x = spawnTile.x;
    pGame->player.y = spawnTile.y;
    pGame->pPlayer->rect.x = spawnTile.x; // windowWidth / 2;
    pGame->pPlayer->rect.y = spawnTile.y; // windowHeight / 2;
    */

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

void loadMedia(SDL_Renderer *pRenderer, SDL_Texture **pPlayerTexture, SDL_Rect gSpriteClips[], int playerID)
{
    static bool textureLoaded = false;

    if (!textureLoaded)
    {
        SDL_Surface *gCatSurface = IMG_Load("resources/cat3.PNG");
        *pPlayerTexture = SDL_CreateTextureFromSurface(pRenderer, gCatSurface);
        textureLoaded = true;
    }
    

    int w = 24, h = 24;
    //playerID--;   //for testing
 
    // IDLE
    gSpriteClips[0].x = 36;
    gSpriteClips[0].y = 34 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[0].w = 24;
    gSpriteClips[0].h = 24;

    gSpriteClips[1].x = 36;
    gSpriteClips[1].y = 290 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[1].w = 24;
    gSpriteClips[1].h = 24;

    gSpriteClips[2].x = 5;
    gSpriteClips[2].y = 482 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE);
    gSpriteClips[2].w = 24;
    gSpriteClips[2].h = 24;

    // RUNNING DOWN
    int x[4] = {644, 676, 708, 740};
    int y[2] = {32 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE), 63 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE)};

    gSpriteClips[3].x = x[0];
    gSpriteClips[3].y = y[1];
    gSpriteClips[3].w = w;
    gSpriteClips[3].h = h;

    gSpriteClips[4].x = x[1];
    gSpriteClips[4].y = y[1];
    gSpriteClips[4].w = w;
    gSpriteClips[4].h = h;

    gSpriteClips[5].x = x[2];
    gSpriteClips[5].y = y[1];
    gSpriteClips[5].w = w;
    gSpriteClips[5].h = h;

    gSpriteClips[6].x = x[3];
    gSpriteClips[6].y = y[1];
    gSpriteClips[6].w = w;
    gSpriteClips[6].h = h;

    gSpriteClips[7].x = x[0];
    gSpriteClips[7].y = y[0];
    gSpriteClips[7].w = w;
    gSpriteClips[7].h = h;

    gSpriteClips[8].x = x[1];
    gSpriteClips[8].y = y[0];
    gSpriteClips[8].w = w;
    gSpriteClips[8].h = h;

    gSpriteClips[9].x = x[2];
    gSpriteClips[9].y = y[0];
    gSpriteClips[9].w = w;
    gSpriteClips[9].h = h;

    gSpriteClips[10].x = x[3];
    gSpriteClips[10].y = y[0];
    gSpriteClips[10].w = w;
    gSpriteClips[10].h = h;

    // RUNNING UP
    y[0] = 289 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE), y[1] = 321 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE);

    gSpriteClips[11].x = x[0];
    gSpriteClips[11].y = y[1];
    gSpriteClips[11].w = w;
    gSpriteClips[11].h = h;

    gSpriteClips[12].x = x[1];
    gSpriteClips[12].y = y[1];
    gSpriteClips[12].w = w;
    gSpriteClips[12].h = h;

    gSpriteClips[13].x = x[2];
    gSpriteClips[13].y = y[1];
    gSpriteClips[13].w = w;
    gSpriteClips[13].h = h;

    gSpriteClips[14].x = x[3];
    gSpriteClips[14].y = y[1];
    gSpriteClips[14].w = w;
    gSpriteClips[14].h = h;

    gSpriteClips[15].x = x[0];
    gSpriteClips[15].y = y[0];
    gSpriteClips[15].w = w;
    gSpriteClips[15].h = h;

    gSpriteClips[16].x = x[1];
    gSpriteClips[16].y = y[0];
    gSpriteClips[16].w = w;
    gSpriteClips[16].h = h;

    gSpriteClips[17].x = x[2];
    gSpriteClips[17].y = y[0];
    gSpriteClips[17].w = w;
    gSpriteClips[17].h = h;

    gSpriteClips[18].x = x[3];
    gSpriteClips[18].y = y[0];
    gSpriteClips[18].w = w;
    gSpriteClips[18].h = h;

    // RUNNING LEFT/RIGHT
    x[0] = 642, x[1] = 674, x[2] = 706, x[3] = 738;
    y[0] = 482 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE), y[1] = 512 + ((playerID - 1) * HEIGHT_OF_PLAYER_SPRITE);

    gSpriteClips[19].x = x[0];
    gSpriteClips[19].y = y[1];
    gSpriteClips[19].w = w;
    gSpriteClips[19].h = h;

    gSpriteClips[20].x = x[1];
    gSpriteClips[20].y = y[1];
    gSpriteClips[20].w = w;
    gSpriteClips[20].h = h;

    gSpriteClips[21].x = x[2];
    gSpriteClips[21].y = y[1];
    gSpriteClips[21].w = w;
    gSpriteClips[21].h = h;

    gSpriteClips[22].x = x[3];
    gSpriteClips[22].y = y[1];
    gSpriteClips[22].w = w;
    gSpriteClips[22].h = h;

    gSpriteClips[23].x = x[0];
    gSpriteClips[23].y = y[0];
    gSpriteClips[23].w = w;
    gSpriteClips[23].h = h;

    gSpriteClips[24].x = x[1];
    gSpriteClips[24].y = y[0];
    gSpriteClips[24].w = w;
    gSpriteClips[24].h = h;

    gSpriteClips[25].x = x[2];
    gSpriteClips[25].y = y[0];
    gSpriteClips[25].w = w;
    gSpriteClips[25].h = h;

    gSpriteClips[26].x = x[3];
    gSpriteClips[26].y = y[0];
    gSpriteClips[26].w = w;
    gSpriteClips[26].h = h;
}

void drawPlayer(Game *pGame, Player player, int i)
{
    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);
    SDL_RenderDrawRect(pGame->pRenderer, &player.rect);
    SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;

    static int frame[MAX_PLAYERS] = {0};
    static int counter[MAX_PLAYERS] = {10, 10, 10, 10, 10};

    switch (player.prevKeyPressed)
    {
    case 'W':
        if (player.idle)
        {
            frame[i] = 0;
            counter[i] = 10;
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[1], &player.rect, 0, NULL, SDL_FLIP_NONE);
        }
        else
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[frame[i] + 3 + 8], &player.rect, 0, NULL, SDL_FLIP_NONE);
        break;
    case 'S':
        if (player.idle)
        {
            frame[i] = 0;
            counter[i] = 10;
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[0], &player.rect, 0, NULL, SDL_FLIP_NONE);
        }
        else
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[frame[i] + 3], &player.rect, 0, NULL, SDL_FLIP_NONE);
        break;
    case 'D':
        if (player.idle)
        {
            frame[i] = 0;
            counter[i] = 10;
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[2], &player.rect, 0, NULL, SDL_FLIP_NONE);
        }
        else
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[frame[i] + 3 + 8 + 8], &player.rect, 0, NULL, SDL_FLIP_NONE);
        break;
    case 'A':
        if (player.idle)
        {
            frame[i] = 0;
            counter[i] = 10;
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[2], &player.rect, 0, NULL, flip);
        }
        else
            SDL_RenderCopyEx(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[frame[i] + 3 + 8 + 8], &player.rect, 0, NULL, flip);
        break;
    }
    if (counter[i] > 0)
        counter[i]--;
    else
    {
        frame[i]++;
        counter[i] = 10;
    }
    if (frame[i] % 8 == 0)
        frame[i] %= 8;
}

Player *createNewMultiPlayer(Game *pGame, int size, PlayerUdpPkg data)
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

    pNew_arr[size].id = data.id;
    pNew_arr[size].x = data.x;
    pNew_arr[size].y = data.y;
    pNew_arr[size].idle = data.idle;
    pNew_arr[size].prevKeyPressed = data.direction;
    pNew_arr[size].charge = data.charge;
    strcpy(pNew_arr[size].name, "Allocated");

    return pNew_arr;
}

void deadPlayer(Game *pGame)
{
}
