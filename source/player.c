#include "definitions.h"
#include "player.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

void centerPlayer(Game *pGame)
{
    int screenShiftAmount = pGame->movementAmount;
    if (pGame->pPlayer->rect.x >= (4 * pGame->windowWidth) / 5 || pGame->pPlayer->rect.x <= pGame->windowWidth / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pGame->pPlayer->rect.y >= (4 * pGame->windowHeight) / 5 || pGame->pPlayer->rect.y <= pGame->windowHeight / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pGame->pPlayer->rect.x >= pGame->windowWidth || pGame->pPlayer->rect.x <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pGame->pPlayer->rect.y >= pGame->windowHeight || pGame->pPlayer->rect.y <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pGame->pPlayer->rect.y < (2 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y += screenShiftAmount;
        }
    }
    if (pGame->pPlayer->rect.y > (3 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y -= screenShiftAmount;
        }
    }
    if (pGame->pPlayer->rect.x < (2 * pGame->windowWidth) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x += screenShiftAmount;
        }
    }
    if (pGame->pPlayer->rect.x > (3 * pGame->windowWidth) / 5)
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
        int damage = 0;

        for (int i = 0; i < 2 * (pGame->pPlayer->charge / 2); i++)
        {
            if (!checkCollision(*pGame->pPlayer, pGame->map, pGame->pPlayer->prevKeyPressed, pGame->world.tileSize))
            {
                movePlayer(pGame->pPlayer, pGame->pPlayer->prevKeyPressed);
            }
            else
            {
                damage = pGame->pPlayer->charge * 2;
                pGame->pPlayer->charge = 1;
                break;
            }
        }
        pGame->pPlayer->hp -= damage;
        pGame->pPlayer->charge -= 1;
    }
    else
    {
        for (int i = 0; i < pGame->movementAmount; i++)
        {
            if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
            {

                if (!checkCollision(*pGame->pPlayer, pGame->map, 'W', pGame->world.tileSize))
                {
                    pGame->pPlayer->prevKeyPressed = 'W';
                    movePlayer(pGame->pPlayer, 'W');
                }
                else
                {
                    // printf("COLLISION W\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
            {

                if (!checkCollision(*pGame->pPlayer, pGame->map, 'A', pGame->world.tileSize))
                {
                    pGame->pPlayer->prevKeyPressed = 'A';
                    movePlayer(pGame->pPlayer, 'A');
                }
                else
                {
                    // printf("COLLISION A\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
            {

                if (!checkCollision(*pGame->pPlayer, pGame->map, 'S', pGame->world.tileSize))
                {
                    pGame->pPlayer->prevKeyPressed = 'S';
                    movePlayer(pGame->pPlayer, 'S');
                }
                else
                {
                    // printf("COLLISION S\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
            {

                if (!checkCollision(*pGame->pPlayer, pGame->map, 'D', pGame->world.tileSize))
                {
                    pGame->pPlayer->prevKeyPressed = 'D';
                    movePlayer(pGame->pPlayer, 'D');
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
    // CENTER PLAYER
    centerPlayer(pGame);

    int offsetX = pGame->map[0].wall.x - pGame->map[0].x;
    int offsetY = pGame->map[0].wall.y - pGame->map[0].y;
    pGame->pPlayer->rect.x = ((float)pGame->pPlayer->x * scaleX) + offsetX;
    pGame->pPlayer->rect.y = ((float)pGame->pPlayer->y * scaleY) + offsetY;

    pGame->pPlayer->rect.h = (pGame->world.tileSize / 2) + ((pGame->world.tileSize / 2) * (1 - (float)pGame->pPlayer->charge / MAX_CHARGE));
    pGame->pPlayer->rect.y += pGame->world.tileSize - pGame->pPlayer->rect.h;
    return 0;
}

void movePlayer(Player *pPlayer, char direction)
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
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        pGame->players[i].rect.x = ((float)pGame->players[i].x * scaleX) + offsetX;
        pGame->players[i].rect.y = ((float)pGame->players[i].y * scaleY) + offsetY;

        pGame->players[i].rect.h = (pGame->world.tileSize / 2) + ((pGame->world.tileSize / 2) * (1 - (float)pGame->players[i].charge / MAX_CHARGE));
        pGame->players[i].rect.y += pGame->world.tileSize - pGame->players[i].rect.h;
    }
}

int changePlayerTexture(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer, char direction)
{
    SDL_Rect srcRect;
    srcRect.x = 611; // test img X starting point
    srcRect.y = 485; // test img Y starting point
    srcRect.w = 24;
    srcRect.h = 24;

    switch (direction) {
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

    SDL_Surface *pSurface = IMG_Load("resources/cat3.png");
    if (!pSurface)
    {
        return -1;
    }
    SDL_Surface *pCroppedSurface = SDL_CreateRGBSurface(0, 24, 24, pSurface->format->BitsPerPixel,
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