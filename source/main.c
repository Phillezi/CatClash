#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "init.h"
#include "definitions.h"

void initMap(Tile map[]);
Tile createTile(int x, int y, int type);

int main(int argv, char **args)
{

    SDL_Init(SDL_INIT_EVERYTHING);
    int fullScreen = 0;
    int collision = 0;
    int movementPrevTime = 0;
    int prevTime = 0;
    int movementAmount = 1;
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 255, blue = 255;
    Tile map[MAPSIZE * MAPSIZE];
    Player player;

    initMap(map);

    SDL_Window *pWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    if (!pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

    SDL_QueryTexture(pTexturePlayer, NULL, NULL, &player.rect.w, &player.rect.h);

    player.rect.x = windowWidth / 2;
    player.rect.y = windowHeight / 2;

    player.rect.w = TILESIZE;
    player.rect.h = TILESIZE;

    int running = 1;
    int oneSecTimer = 0, frameCounter = 0;

    while (running)
    {
        if (SDL_GetTicks() - oneSecTimer >= 1000) // Performance monitor
        {
            oneSecTimer = SDL_GetTicks();
            printf("FPS: %d\n", frameCounter);
            frameCounter = 0;
        }
        int deltaTime = SDL_GetTicks() - prevTime;
        if (deltaTime >= 1000 / FPS) // updates at a frequency of FPS
        {
            prevTime = SDL_GetTicks();

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = 0;
            }

            int movementDeltaTime = SDL_GetTicks() - movementPrevTime;
            if (movementDeltaTime >= 1000 / 60) // updates every ~60 times per second
            {
                movementPrevTime = SDL_GetTicks();

                const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
                if (currentKeyStates[SDL_SCANCODE_R] || currentKeyStates[SDL_SCANCODE_DELETE])
                {
                    initMap(map);
                    player.rect.x = windowWidth / 2;
                    player.rect.y = windowHeight / 2;
                    printf("Reset\n");
                }
                if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
                {
                    collision = 0;
                    if (map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].type != 0)
                    {
                        map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].type = 2;
                        collision = 1;

                        printf("DEBUG: COLLISION W at player(%d,%d) and tile(%d,%d)\n", player.rect.x, player.rect.y, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.x, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.y);
                    }
                    else if (map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE-1)) / TILESIZE)].type != 0){
                        map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE-1)) / TILESIZE)].type = 2;
                        collision = 1;
                    }

                    if (!collision)
                    {
                        if (1) // playerRect.y >= windowHeight / 4)
                        {
                            player.rect.y -= movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.y += movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
                {
                    // moveUp(map, playerRect, movementAmount, TILESIZE, MAPSIZE)

                    collision = 0;
                    if (map[((player.rect.y / TILESIZE) * MAPSIZE) + ((player.rect.x - movementAmount) / TILESIZE)].type != 0)
                    {
                        map[((player.rect.y / TILESIZE) * MAPSIZE) + ((player.rect.x - movementAmount) / TILESIZE)].type = 2;
                        collision = 1;
                        printf("DEBUG: COLLISION A at player(%d,%d) and tile(%d,%d)\n", player.rect.x, player.rect.y, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.x, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.y);
                    } else if (map[(((player.rect.y + TILESIZE - 1)/ TILESIZE) * MAPSIZE) + ((player.rect.x - movementAmount) / TILESIZE)].type != 0)
                    {
                        map[(((player.rect.y + TILESIZE - 1) / TILESIZE) * MAPSIZE) + ((player.rect.x - movementAmount) / TILESIZE)].type = 2;
                        collision = 1;
                    }

                    if (!collision)
                    {
                        if (1) // playerRect.x >= windowWidth / 4)
                        {
                            player.rect.x -= movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.x += movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
                {
                    collision = 0;
                    if (map[(((player.rect.y + (TILESIZE - 1) + movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].type != 0)
                    {
                        map[(((player.rect.y + (TILESIZE - 1) + movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].type = 2;
                        collision = 1;
                        printf("DEBUG: COLLISION S at player(%d,%d) and tile(%d,%d)\n", player.rect.x, player.rect.y, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.x, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.y);
                    }
                    else if (map[(((player.rect.y + (TILESIZE - 1) + movementAmount) / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE-1)) / TILESIZE)].type != 0){
                        map[(((player.rect.y + (TILESIZE - 1) + movementAmount) / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE-1)) / TILESIZE)].type = 2;
                        collision = 1;
                    }

                    if (!collision)
                    {
                        if (1) // playerRect.y <= ((3 * windowHeight) / 4))
                        {
                            player.rect.y += movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.y -= movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
                {
                    collision = 0;
                    if (map[((player.rect.y / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE - 1) + movementAmount) / TILESIZE)].type != 0)
                    {
                        map[((player.rect.y / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE - 1) + movementAmount) / TILESIZE)].type = 2;
                        collision = 1;
                        printf("DEBUG: COLLISION D at player(%d,%d) and tile(%d,%d)\n", player.rect.x, player.rect.y, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.x, map[(((player.rect.y - movementAmount) / TILESIZE) * MAPSIZE) + (player.rect.x / TILESIZE)].wall.y);
                    } else if (map[(((player.rect.y + TILESIZE - 1)/ TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE - 1) + movementAmount) / TILESIZE)].type != 0)
                    {
                        map[(((player.rect.y + TILESIZE - 1) / TILESIZE) * MAPSIZE) + ((player.rect.x + (TILESIZE - 1) + movementAmount) / TILESIZE)].type = 2;
                        collision = 1;
                    }

                    if (!collision)
                    {
                        if (1) // playerRect.x <= ((3 * windowWidth) / 4))
                        {
                            player.rect.x += movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.x -= movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_F11])
                {
                    if (!fullScreen)
                    {
                        SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN);
                        fullScreen = 1;
                    }
                    else
                    {
                        SDL_SetWindowFullscreen(pWindow, 0);
                        fullScreen = 0;
                    }
                }
            }

            SDL_SetRenderDrawColor(pRenderer, red, green, blue, 255);
            SDL_RenderClear(pRenderer);
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                switch (map[i].type)
                {
                case 0:
                    break;
                case 1:
                    // SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[0], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 2:
                    // SDL_SetRenderDrawColor(pRenderer, 100, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[1], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 3:
                    // SDL_SetRenderDrawColor(pRenderer, 0, 100, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[2], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 4:
                    // SDL_SetRenderDrawColor(pRenderer, 0, 0, 100, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[3], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                default:
                    break;
                }
            }

            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(pRenderer, &player.rect);
            SDL_RenderCopy(pRenderer, pTexturePlayer, NULL, &player.rect);
            SDL_RenderPresent(pRenderer);
            frameCounter++;
        }
    }
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

void initMap(Tile map[])
{
    int type = 0;
    // READ FROM map.txt
    FILE *fp;
    fp = fopen(MAP_FILEPATH, "r");
    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                type = 0;
                fscanf(fp, "%d", &type);
                map[row * MAPSIZE + col] = createTile(col * TILESIZE, row * TILESIZE, type);
            }
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR READING FILE");
    }
}
Tile createTile(int x, int y, int type)
{
    Tile i;
    i.wall.x = x;
    i.wall.y = y;
    i.wall.w = TILESIZE;
    i.wall.h = TILESIZE;
    i.type = type;
    return i;
}