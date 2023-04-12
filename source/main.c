#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>
#include "init.h"
#include "definitions.h"
#include "menu.h"

#define MAX_CHARGE 100

void updateScreen(SDL_Renderer *pRenderer, Player player, Tile map[], SDL_Texture *pTextureTiles[], SDL_Texture *pTexturePlayer);
int handleInput(Player *pPlayer, Tile map[], int movementAmount, int *pCharge);
void movePlayer(Player *pPlayer, char direction);
int checkCollision(Player player, Tile map[], char direction);
SDL_Rect findEmptyTile(Tile map[]);

int main(int argv, char **args)
{
    bool run = true;
    int previousTime = 0, movementPreviousTime = 0, charge = 0;
    Player *pPlayer;
    Tile map[MAPSIZE * MAPSIZE];
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
    {
        printf("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
    }
    else
    {
        windowWidth = dm.h - 100, windowHeight = dm.h - 100;
    }

    int tileSize = (windowHeight / MAPSIZE);

    char fileName[31];
    do
    {
        printf("map name: ");
        scanf(" %30s", fileName);
    } while (initMap(map, fileName, tileSize) == -1);

    // int windowWidth = dm.w, windowHeight = dm.h; //DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    SDL_Window *pWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    if (!pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED); // | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    char tileTextures[TILES][20] = {"resources/Tile1.png", "resources/Tile2.png", "resources/Tile3.png", "resources/Tile4.png"};
    SDL_Texture *pTextureTiles[TILES];
    if (initTextureTiles(pRenderer, pWindow, pTextureTiles, tileTextures, TILES) == -1)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTexturePlayer;
    if (initTexturePlayer(pRenderer, pWindow, &pTexturePlayer) == -1)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_QueryTexture(pTexturePlayer, NULL, NULL, &pPlayer->rect.w, &pPlayer->rect.h);
    SDL_Rect spawnTile = findEmptyTile(map);
    pPlayer->rect.x = spawnTile.x; // windowWidth / 2;
    pPlayer->rect.y = spawnTile.y; // windowHeight / 2;

    pPlayer->rect.w = map[0].wall.w;
    pPlayer->rect.h = map[0].wall.w;

    while (run)
    {
        int movementDeltaTime = SDL_GetTicks() - movementPreviousTime;
        if (movementDeltaTime >= (1000 / 60))
        {
            movementPreviousTime = SDL_GetTicks();
            handleInput(pPlayer, map, 5, &charge);

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    run = false;
            }
        }
        int deltaTime = SDL_GetTicks() - previousTime;
        if (deltaTime >= (1000 / FPS))
        {
            previousTime = SDL_GetTicks();
            updateScreen(pRenderer, *pPlayer, map, pTextureTiles, pTexturePlayer);
        }
    }
    // DESTROY EVERYTHING
    SDL_DestroyTexture(pTextureTiles[0]);
    SDL_DestroyTexture(pTextureTiles[1]);
    SDL_DestroyTexture(pTextureTiles[2]);
    SDL_DestroyTexture(pTextureTiles[3]);
    SDL_DestroyTexture(pTexturePlayer);

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}
void updateScreen(SDL_Renderer *pRenderer, Player player, Tile map[], SDL_Texture *pTextureTiles[], SDL_Texture *pTexturePlayer)
{
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pRenderer);
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        switch (map[i].type)
        {
        case 0:
            break;
        case 1:
            SDL_RenderCopy(pRenderer, pTextureTiles[0], NULL, &map[i].wall);
            break;
        case 2:
            SDL_RenderCopy(pRenderer, pTextureTiles[1], NULL, &map[i].wall);
            break;
        case 3:
            SDL_RenderCopy(pRenderer, pTextureTiles[2], NULL, &map[i].wall);
            break;
        case 4:
            SDL_RenderCopy(pRenderer, pTextureTiles[3], NULL, &map[i].wall);
            break;
        default:
            break;
        }
    }
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(pRenderer, &player.rect);
    SDL_RenderCopy(pRenderer, pTexturePlayer, NULL, &player.rect);
    SDL_RenderPresent(pRenderer);
}
int handleInput(Player *pPlayer, Tile map[], int movementAmount, int *pCharge)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    if (currentKeyStates[SDL_SCANCODE_SPACE])
    {
        if (*pCharge < MAX_CHARGE)
        {
            *pCharge += 1;
        }
        else
        {
            printf("FULLY CHARGED\n");
        }
    }
    else if (*pCharge > 0)
    {
        for (int i = 0; i < 10; i++)
        {
            if (!checkCollision(*pPlayer, map, 'W'))
            {
                movePlayer(pPlayer, 'W');
            }
            else
            {
                pPlayer->hp -= 1;
            }
        }
        *pCharge -= 1;
    }
    else
    {
        for (int i = 0; i < movementAmount; i++)
        {
            if (currentKeyStates[SDL_SCANCODE_W])
            {
                if (!checkCollision(*pPlayer, map, 'W'))
                {
                    movePlayer(pPlayer, 'W');
                }
                else
                {
                    printf("COLLISION W\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A])
            {
                if (!checkCollision(*pPlayer, map, 'A'))
                {
                    movePlayer(pPlayer, 'A');
                }
                else
                {
                    printf("COLLISION A\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S])
            {
                if (!checkCollision(*pPlayer, map, 'S'))
                {
                    movePlayer(pPlayer, 'S');
                }
                else
                {
                    printf("COLLISION S\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D])
            {
                if (!checkCollision(*pPlayer, map, 'D'))
                {
                    movePlayer(pPlayer, 'D');
                }
                else
                {
                    printf("COLLISION D\n");
                }
            }
        }
    }
    return 0;
}
void movePlayer(Player *pPlayer, char direction)
{
    switch (direction)
    {
    case 'W':
        pPlayer->rect.y--;
        break;
    case 'A':
        pPlayer->rect.x--;
        break;
    case 'S':
        pPlayer->rect.y++;
        break;
    case 'D':
        pPlayer->rect.x++;
        break;
    default:
        break;
    }
}
int checkCollision(Player player, Tile map[], char direction)
{
    switch (direction)
    {
    case 'W':
        if (map[(((player.rect.y - 1) / map[0].wall.w) * MAPSIZE) + (player.rect.x / map[0].wall.w)].type)
            return map[(((player.rect.y - 1) / map[0].wall.w) * MAPSIZE) + (player.rect.x / map[0].wall.w)].type;
        else if (map[(((player.rect.y - 1) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + (map[0].wall.w - 1)) / map[0].wall.w)].type)
            return map[(((player.rect.y - 1) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + (map[0].wall.w - 1)) / map[0].wall.w)].type;
        break;
    case 'A':
        if (map[(((player.rect.y) / map[0].wall.w) * MAPSIZE) + ((player.rect.x - 1) / map[0].wall.w)].type)
            return map[(((player.rect.y) / map[0].wall.w) * MAPSIZE) + ((player.rect.x - 1) / map[0].wall.w)].type;
        else if (map[(((player.rect.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.rect.x - 1) / map[0].wall.w)].type)
            return map[(((player.rect.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.rect.x - 1) / map[0].wall.w)].type;
        break;
    case 'S':
        if (map[(((player.rect.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + (player.rect.x / map[0].wall.w)].type)
            return map[(((player.rect.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + (player.rect.x / map[0].wall.w)].type;
        else if (map[(((player.rect.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + (map[0].wall.w - 1)) / map[0].wall.w)].type)
            return map[(((player.rect.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + (map[0].wall.w - 1)) / map[0].wall.w)].type;
        break;
    case 'D':
        if (map[(((player.rect.y) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + map[0].wall.w) / map[0].wall.w)].type)
            return map[(((player.rect.y) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + map[0].wall.w) / map[0].wall.w)].type;
        else if (map[(((player.rect.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + map[0].wall.w) / map[0].wall.w)].type)
            return map[(((player.rect.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.rect.x + map[0].wall.w) / map[0].wall.w)].type;
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