#include "definitions.h"
#include "player.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

void centerPlayer(Game *pGame)
{
    int screenShiftAmount = pGame->movementAmount;
    if (pGame->player.rect.x >= (4 * pGame->windowWidth) / 5 || pGame->player.rect.x <= pGame->windowWidth / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pGame->player.rect.y >= (4 * pGame->windowHeight) / 5 || pGame->player.rect.y <= pGame->windowHeight / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pGame->player.rect.x >= pGame->windowWidth || pGame->player.rect.x <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pGame->player.rect.y >= pGame->windowHeight || pGame->player.rect.y <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pGame->player.rect.y < (2 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y += screenShiftAmount;
        }
    }
    if (pGame->player.rect.y > (3 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y -= screenShiftAmount;
        }
    }
    if (pGame->player.rect.x < (2 * pGame->windowWidth) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x += screenShiftAmount;
        }
    }
    if (pGame->player.rect.x > (3 * pGame->windowWidth) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x -= screenShiftAmount;
        }
    }
}

int handleInput(Game *pGame)
{
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
        if (pGame->player.charge < MAX_CHARGE)
        {
            pGame->player.charge += 1;
        }
        else
        {
            printf("FULLY CHARGED\n");
        }
    }
    else if (pGame->player.charge > 0)
    {
        int damage = 0;

        for (int i = 0; i < 2 * (pGame->player.charge / 2); i++)
        {
            if (!checkCollision(pGame->player, pGame->map, pGame->player.prevKeyPressed, pGame->world.tileSize))
            {
                movePlayer(&pGame->player, pGame->player.prevKeyPressed);
            }
            else
            {
                damage = pGame->player.charge * 2;
                pGame->player.charge = 0;
                break;
            }
        }
        pGame->player.hp -= damage;
        pGame->player.charge -= 1;
    }
    else
    {
        for (int i = 0; i < pGame->movementAmount; i++)
        {
            if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
            {
                pGame->player.prevKeyPressed = 'W';
                if (!checkCollision(pGame->player, pGame->map, 'W', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'W');
                }
                else
                {
                    printf("COLLISION W\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
            {
                pGame->player.prevKeyPressed = 'A';
                if (!checkCollision(pGame->player, pGame->map, 'A', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'A');
                }
                else
                {
                    printf("COLLISION A\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
            {
                pGame->player.prevKeyPressed = 'S';
                if (!checkCollision(pGame->player, pGame->map, 'S', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'S');
                }
                else
                {
                    printf("COLLISION S\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
            {
                pGame->player.prevKeyPressed = 'D';
                if (!checkCollision(pGame->player, pGame->map, 'D', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'D');
                }
                else
                {
                    printf("COLLISION D\n");
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
    pGame->player.rect.x = ((float)pGame->player.x * scaleX) + offsetX;
    pGame->player.rect.y = ((float)pGame->player.y * scaleY) + offsetY;
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

void getPlayerSpawnPos(Game *pGame)
{
    SDL_Rect spawnTile = findEmptyTile(pGame->map); // this function returns a valid spawn tile
    pGame->player.x = spawnTile.x;
    pGame->player.y = spawnTile.y;
    pGame->player.rect.x = spawnTile.x; // windowWidth / 2;
    pGame->player.rect.y = spawnTile.y; // windowHeight / 2;
}
