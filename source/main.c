#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "init.h"
#include "definitions.h"
// #include "menu.h"

#define MAX_CHARGE 100

struct uiElements
{
    SDL_Rect chargebar;
    SDL_Rect healthbar;
    SDL_Rect fpsFrame;
};
typedef struct uiElements UiE;

void updateScreen(SDL_Renderer *pRenderer, Player player, Tile map[], SDL_Texture *pTextureTiles[], SDL_Texture *pTexturePlayer, UiE ui, SDL_Texture *pFpsTexture, float angle, int tileSize);
int handleInput(Player *pPlayer, Tile map[], int movementAmount, int *pCharge, char *pPrevKeyPressed, float *pAngle, int tileSize);
void movePlayer(Player *pPlayer, char direction);
int checkCollision(Player player, Tile map[], char direction);
SDL_Rect findEmptyTile(Tile map[]);

int main(int argv, char **args)
{
    bool run = true;
    int previousTime = 0, movementPreviousTime = 0, charge = 0;
    Player *pPlayer = malloc(sizeof(Player));
    if (pPlayer == NULL)
    {
        printf("Error: Failed to allocate memory for pPlayer\n");
        return 1;
    }

    Tile map[MAPSIZE * MAPSIZE];
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    char prevKeyPressed;
    UiE ui;

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

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

    if (SDL_QueryTexture(pTexturePlayer, NULL, NULL, &pPlayer->rect.w, &pPlayer->rect.h) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Rect spawnTile = findEmptyTile(map); // this function returns a valid spawn tile
    pPlayer->x = spawnTile.x;
    pPlayer->y = spawnTile.y;
    pPlayer->rect.x = spawnTile.x; // windowWidth / 2;
    pPlayer->rect.y = spawnTile.y; // windowHeight / 2;

    pPlayer->rect.w = map[0].wall.w;
    pPlayer->rect.h = map[0].wall.w;

    pPlayer->hp = 255;

    ui.chargebar.x = ((windowWidth / 2) - (MAX_CHARGE / 2));
    ui.chargebar.y = ((3 * windowHeight) / 4);
    ui.chargebar.w = charge;
    ui.chargebar.h = map[0].wall.w;

    ui.healthbar.x = ((windowWidth / 2) + (MAX_CHARGE / 2) + 5);
    ui.healthbar.y = ((3 * windowHeight) / 4);
    ui.healthbar.w = pPlayer->hp;
    ui.healthbar.h = map[0].wall.w;

    // PERFORMANCE monitor
    int oneSecTimer = 0, frameCounter = 0;
    ui.fpsFrame.w = 40;
    ui.fpsFrame.h = 40;
    ui.fpsFrame.x = windowWidth - ui.fpsFrame.w;
    ui.fpsFrame.y = 0;

    TTF_Font *pFont = TTF_OpenFont("resources/fonts/Pixelletters.ttf", 20);
    SDL_Color colGreen = {0, 255, 0};
    SDL_Surface *pFpsSurface = TTF_RenderText_Solid(pFont, "FPS", colGreen);
    SDL_Texture *pFpsTexture = SDL_CreateTextureFromSurface(pRenderer, pFpsSurface);
    SDL_FreeSurface(pFpsSurface);
    float angle = 0;

    while (run)
    {
        if (SDL_GetTicks() - oneSecTimer >= 1000) // Performance monitor
        {
            oneSecTimer = SDL_GetTicks();
            char buffer[50];
            SDL_DestroyTexture(pFpsTexture);
            sprintf(buffer, "%d", frameCounter);
            SDL_Surface *pFpsSurface = TTF_RenderText_Solid(pFont, buffer, colGreen);
            SDL_Texture *pFpsTexture = SDL_CreateTextureFromSurface(pRenderer, pFpsSurface);
            SDL_FreeSurface(pFpsSurface);
            printf("%s\n", buffer);
            frameCounter = 0;
        }
        int deltaTime = SDL_GetTicks() - previousTime;
        if (deltaTime >= (1000 / FPS))
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    run = false;
            }

            int movementDeltaTime = SDL_GetTicks() - movementPreviousTime;
            if (movementDeltaTime >= (1000 / 60))
            {
                movementPreviousTime = SDL_GetTicks();
                handleInput(pPlayer, map, 5, &charge, &prevKeyPressed, &angle, tileSize);
                // printf("ANGLE: %f\n", angle);

                if (pPlayer->hp <= 0)
                {
                    printf("You Died\n");
                    pPlayer->hp = 255;
                }

                ui.healthbar.w = pPlayer->hp;
                ui.chargebar.w = charge;
            }
            previousTime = SDL_GetTicks();
            updateScreen(pRenderer, *pPlayer, map, pTextureTiles, pTexturePlayer, ui, pFpsTexture, angle, tileSize);
            frameCounter++;
        }
    }
    // DESTROY EVERYTHING
    free(pPlayer);
    SDL_DestroyTexture(pTextureTiles[0]);
    SDL_DestroyTexture(pTextureTiles[1]);
    SDL_DestroyTexture(pTextureTiles[2]);
    SDL_DestroyTexture(pTextureTiles[3]);
    SDL_DestroyTexture(pTexturePlayer);
    SDL_DestroyTexture(pFpsTexture);

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_CloseFont(pFont);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
void updateScreen(SDL_Renderer *pRenderer, Player player, Tile map[], SDL_Texture *pTextureTiles[], SDL_Texture *pTexturePlayer, UiE ui, SDL_Texture *pFpsTexture, float angle, int tileSize)
{
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pRenderer);
    SDL_Rect temp;
    for (int i = 0; i < (((player.y) / map[0].wall.w) * MAPSIZE) + ((player.x - 1) / map[0].wall.w) + 2; i++)
    {
        switch (map[i].type)
        {
        case 0:
            if (i > MAPSIZE - 1)
            {
                if (map[i - MAPSIZE].type)
                {
                    temp = map[i].wall;
                    temp.h = ((float)tileSize * angle);
                    SDL_SetTextureColorMod(pTextureTiles[(map[i - MAPSIZE].type - 1)], 150, 150, 150);
                    SDL_RenderCopy(pRenderer, pTextureTiles[(map[i - MAPSIZE].type - 1)], NULL, &temp);
                    SDL_SetTextureColorMod(pTextureTiles[(map[i - MAPSIZE].type - 1)], 255, 255, 255);
                }
            }
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

    for (int i = (((player.y) / map[0].wall.w) * MAPSIZE) + ((player.x - 1) / map[0].wall.w) + 2; i < MAPSIZE * MAPSIZE; i++)
    {
        switch (map[i].type)
        {
        case 0:
            if (i > MAPSIZE - 1)
            {
                if (map[i - MAPSIZE].type)
                {
                    temp = map[i].wall;
                    temp.h = ((float)tileSize * angle);
                    SDL_SetTextureColorMod(pTextureTiles[(map[i - MAPSIZE].type - 1)], 150, 150, 150);
                    SDL_RenderCopy(pRenderer, pTextureTiles[(map[i - MAPSIZE].type - 1)], NULL, &temp);
                    SDL_SetTextureColorMod(pTextureTiles[(map[i - MAPSIZE].type - 1)], 255, 255, 255);
                }
            }
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

    SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
    SDL_RenderFillRect(pRenderer, &ui.healthbar);

    SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(pRenderer, &ui.chargebar);

    SDL_RenderCopy(pRenderer, pFpsTexture, NULL, &ui.fpsFrame);

    SDL_RenderPresent(pRenderer);
}
int handleInput(Player *pPlayer, Tile map[], int movementAmount, int *pCharge, char *pPrevKeypressed, float *pAngle, int tileSize)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    float scale = (float)map[0].wall.h / tileSize;
    if (currentKeyStates[SDL_SCANCODE_Q])
    {
        if (*pAngle >= 0.01)
        {
            *pAngle -= 0.01;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                map[i].wall.h = ((float)tileSize * (1 - *pAngle));

                if (i > (MAPSIZE - 1))
                {
                    map[i].wall.y = map[i - MAPSIZE].wall.y + map[i].wall.h;
                }
            }
            pPlayer->rect.y = (float)pPlayer->y * (float)map[0].wall.h / tileSize;
        }
    }
    if (currentKeyStates[SDL_SCANCODE_E])
    {
        if (*pAngle < 1)
        {
            *pAngle += 0.01;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                map[i].wall.h = ((float)tileSize * (1 - *pAngle));

                if (i > (MAPSIZE - 1))
                {
                    map[i].wall.y = map[i - MAPSIZE].wall.y + map[i].wall.h;
                }
            }
            pPlayer->rect.y = (float)pPlayer->y * (float)map[0].wall.h / tileSize;
        }
    }
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
        int damage = 0;

        for (int i = 0; i < 2 * (*pCharge / 2); i++)
        {
            if (!checkCollision(*pPlayer, map, *pPrevKeypressed))
            {
                movePlayer(pPlayer, *pPrevKeypressed);
                pPlayer->rect.y = (float)pPlayer->y * scale;
            }
            else
            {
                damage = *pCharge * 2;
                *pCharge = 0;
                break;
            }
        }
        pPlayer->hp -= damage;
        *pCharge -= 1;
    }
    else
    {
        for (int i = 0; i < movementAmount; i++)
        {
            if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
            {
                *pPrevKeypressed = 'W';
                if (!checkCollision(*pPlayer, map, 'W'))
                {
                    movePlayer(pPlayer, 'W');
                    pPlayer->rect.y = (float)pPlayer->y * scale;
                }
                else
                {
                    printf("COLLISION W\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
            {
                *pPrevKeypressed = 'A';
                if (!checkCollision(*pPlayer, map, 'A'))
                {
                    movePlayer(pPlayer, 'A');
                    // pPlayer->rect.y = (float)pPlayer->y * scale;
                }
                else
                {
                    printf("COLLISION A\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
            {
                *pPrevKeypressed = 'S';
                if (!checkCollision(*pPlayer, map, 'S'))
                {
                    movePlayer(pPlayer, 'S');
                    pPlayer->rect.y = (float)pPlayer->y * scale;
                }
                else
                {
                    printf("COLLISION S\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
            {
                *pPrevKeypressed = 'D';
                if (!checkCollision(*pPlayer, map, 'D'))
                {
                    movePlayer(pPlayer, 'D');
                    // pPlayer->rect.y = pPlayer->y * scale;
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
    return 0;
}
void movePlayer(Player *pPlayer, char direction)
{
    switch (direction)
    {
    case 'W':
        // pPlayer->rect.y--;
        pPlayer->y--;
        break;
    case 'A':
        pPlayer->rect.x--;
        pPlayer->x--;
        break;
    case 'S':
        // pPlayer->rect.y++;
        pPlayer->y++;
        break;
    case 'D':
        pPlayer->rect.x++;
        pPlayer->x++;
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
        if (map[(((player.y - 1) / map[0].wall.w) * MAPSIZE) + (player.x / map[0].wall.w)].type)
            return map[(((player.y - 1) / map[0].wall.w) * MAPSIZE) + (player.x / map[0].wall.w)].type;
        else if (map[(((player.y - 1) / map[0].wall.w) * MAPSIZE) + ((player.x + (map[0].wall.w - 1)) / map[0].wall.w)].type)
            return map[(((player.y - 1) / map[0].wall.w) * MAPSIZE) + ((player.x + (map[0].wall.w - 1)) / map[0].wall.w)].type;
        break;
    case 'A':
        if (map[(((player.y) / map[0].wall.w) * MAPSIZE) + ((player.x - 1) / map[0].wall.w)].type)
            return map[(((player.y) / map[0].wall.w) * MAPSIZE) + ((player.x - 1) / map[0].wall.w)].type;
        else if (map[(((player.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.x - 1) / map[0].wall.w)].type)
            return map[(((player.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.x - 1) / map[0].wall.w)].type;
        break;
    case 'S':
        if (map[(((player.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + (player.x / map[0].wall.w)].type)
            return map[(((player.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + (player.x / map[0].wall.w)].type;
        else if (map[(((player.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + ((player.x + (map[0].wall.w - 1)) / map[0].wall.w)].type)
            return map[(((player.y + map[0].wall.w) / map[0].wall.w) * MAPSIZE) + ((player.x + (map[0].wall.w - 1)) / map[0].wall.w)].type;
        break;
    case 'D':
        if (map[(((player.y) / map[0].wall.w) * MAPSIZE) + ((player.x + map[0].wall.w) / map[0].wall.w)].type)
            return map[(((player.y) / map[0].wall.w) * MAPSIZE) + ((player.x + map[0].wall.w) / map[0].wall.w)].type;
        else if (map[(((player.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.x + map[0].wall.w) / map[0].wall.w)].type)
            return map[(((player.y + (map[0].wall.w - 1)) / map[0].wall.w) * MAPSIZE) + ((player.x + map[0].wall.w) / map[0].wall.w)].type;
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