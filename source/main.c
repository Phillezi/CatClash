/*
    KITTENFORK main.c HI1038
*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include "engine.h"

// Network
#include <SDL2/SDL_net.h>
#include "client.h"
#include "server.h"

#define CONFIG_FILEPATH "resources/config.txt"
#define MAP_FILEPATH "resources/map.txt"
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define WINDOW_NAME "Kitten Game"
#define FPS 165
#define MAPSIZE 32
#define TILESIZE 16 * 10
// #define TILESIZE 33

struct wall
{
    SDL_Rect wall;
};
typedef struct wall Wall;

void initConfig(int *pWindowWidth, int *pWindowHeight);
void initMap(int map[][MAPSIZE]);
Wall createWall(int row, int col);

int main(int argv, char **args)
{

    int prevTime = 0, movementPrevTime = 0;
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 255, blue = 255;
    int map[MAPSIZE][MAPSIZE];
    int nrOfWalls = 0;
    Wall walls[MAPSIZE * MAPSIZE];

    initConfig(&windowWidth, &windowHeight);
    initMap(map);

    for (int row = 0; row < MAPSIZE; row++)
    {
        for (int col = 0; col < MAPSIZE; col++)
        {
            switch (map[row][col])
            {
            case 0:
                break;
            case 1:
                walls[nrOfWalls] = createWall(row, col);
                nrOfWalls++;
                break;
            case 2:
                break; // walls[nrOfWalls] = createWall(row, col); nrOfWalls++; break;
            case 3:
                break; // walls[nrOfWalls] = createWall(row, col); nrOfWalls++; break;
            case 4:
                break; // walls[nrOfWalls] = createWall(row, col); nrOfWalls++; break;
            default:
                break; // walls[nrOfWalls] = createWall(row, col); nrOfWalls++; break;
            }
        }
    }

    printf("Your current resolution is %dx%d", windowWidth, windowHeight);

    SDL_Point vPoints[(windowWidth / 64) * 2 + 6];
    for (int i = 0; i < ((windowWidth / 64) * 2 + 5); i++)
    {
        vPoints[i].y = 0;
        vPoints[i].x = i * 64;
        i++;
        vPoints[i].y = windowHeight;
        vPoints[i].x = (i - 1) * 64;
    }
    int vLineCount = windowWidth / 64 + 3;

    SDL_Point hPoints[(windowHeight / 64) * 2 + 6];
    for (int i = 0; i < ((windowHeight / 64) * 2 + 5); i++)
    {
        hPoints[i].x = 0;
        hPoints[i].y = i * 64;
        i++;
        hPoints[i].x = windowWidth;
        hPoints[i].y = (i - 1) * 64;
    }
    int hLineCount = windowHeight / 64 + 3;

    // SDL_Window *pWindow = initWindow(WINDOW_NAME, windowWidth, windowHeight);
    // SDL_Renderer *pRenderer = initRenderer(pWindow);

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

    SDL_Surface *pSurface = IMG_Load("resources/cat3.png");
    if (!pSurface)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if (!pTexture)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Rect playerRect;
    SDL_QueryTexture(pTexture, NULL, NULL, &playerRect.w, &playerRect.h);

    playerRect.x = 0;//TILESIZE;//windowWidth / 2;
    playerRect.y = 0;//TILESIZE;//windowHeight / 2;

    playerRect.w = TILESIZE;
    playerRect.h = TILESIZE;

    int movementAmount = 1;
    int colorChanger = 1;

    int running = 1;

    while (running)
    {
        int deltaTime = SDL_GetTicks() - prevTime;
        if (deltaTime >= 1000 / FPS) // updates at a frequency of FPS
        {
            prevTime = SDL_GetTicks();
            // red += 2;
            green += colorChanger;
            blue += colorChanger;

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = 0;
            }
            int movementDeltaTime = SDL_GetTicks() - movementPrevTime;
            if (movementDeltaTime >= 1000 / 60)
            {
                movementPrevTime = SDL_GetTicks();
                const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

                if (currentKeyStates[SDL_SCANCODE_W])
                {
                    int validPos = 1;
                    for (int i = 0; i < nrOfWalls; i++)
                    {
                        if ((((playerRect.y - movementAmount) >= walls[i].wall.y) && ((playerRect.y - movementAmount) <= (walls[i].wall.y + TILESIZE))) && ((playerRect.x >= walls[i].wall.x) && (playerRect.x <= walls[i].wall.x + TILESIZE)))
                        {
                            validPos = 0;
                        }
                    }
                    if (validPos)
                    {
                        if (playerRect.y >= windowHeight / 4)
                        {
                            playerRect.y -= movementAmount;
                        }
                        else
                        {
                            /*
                            for (int i = 0; i <= hLineCount; i++)
                            {
                                hPoints[i].y += movementAmount;
                            }
                            */
                            for (int i = 0; i < nrOfWalls; i++)
                            {
                                walls[i].wall.y += movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_S])
                {
                    int validPos = 1;
                    for (int i = 0; i < nrOfWalls; i++)
                    {
                        if ((((playerRect.y + TILESIZE + movementAmount) >= walls[i].wall.y) && ((playerRect.y + TILESIZE + movementAmount) <= (walls[i].wall.y + TILESIZE))) && ((playerRect.x >= walls[i].wall.x) && (playerRect.x <= walls[i].wall.x + TILESIZE)))
                        {
                            validPos = 0;
                        }
                    }
                    if (validPos)
                    {
                        if (playerRect.y <= ((3 * windowHeight) / 4))
                        {
                            playerRect.y += movementAmount;
                        }
                        else
                        {
                            /*
                            for (int i = 0; i <= hLineCount; i++)
                            {
                                hPoints[i].y -= movementAmount;
                            }
                            */
                            for (int i = 0; i < nrOfWalls; i++)
                            {
                                walls[i].wall.y -= movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_A])
                {
                    int validPos = 1;
                    for (int i = 0; i < nrOfWalls; i++)
                    {
                        if ((((playerRect.x - movementAmount) >= walls[i].wall.x) && ((playerRect.x - movementAmount) <= (walls[i].wall.x + TILESIZE)))&& ((playerRect.y >= walls[i].wall.y) && (playerRect.y <= walls[i].wall.y + TILESIZE)))
                        {
                            validPos = 0;
                        }
                    }
                    if (validPos)
                    {
                        if (playerRect.x >= (windowWidth / 4))
                        {
                            playerRect.x -= movementAmount;
                        }
                        else
                        {
                            /*
                            for (int i = 0; i <= vLineCount; i++)
                            {
                                vPoints[i].x += movementAmount;
                            }
                            */
                            for (int i = 0; i < nrOfWalls; i++)
                            {
                                walls[i].wall.x += movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_D])
                {
                    int validPos = 1;
                    for (int i = 0; i < nrOfWalls; i++)
                    {
                        if ((((playerRect.x + TILESIZE + movementAmount) >= walls[i].wall.x) && ((playerRect.x + TILESIZE + movementAmount) <= (walls[i].wall.x + TILESIZE)))&& ((playerRect.y >= walls[i].wall.y) && (playerRect.y <= walls[i].wall.y + TILESIZE)))
                        {
                            validPos = 0;
                        }
                    }
                    if (validPos)
                    {
                        if (playerRect.x <= ((3 * windowWidth) / 4))
                        {
                            playerRect.x += movementAmount;
                        }
                        else
                        {
                            /*
                            for (int i = 0; i <= vLineCount; i++)
                            {
                                vPoints[i].x -= movementAmount;
                            }
                            */
                            for (int i = 0; i < nrOfWalls; i++)
                            {
                                walls[i].wall.x -= movementAmount;
                            }
                        }
                    }
                }

                if (hPoints[0].y > 128)
                {
                    for (int i = 0; i <= hLineCount; i++)
                    {
                        hPoints[i].y = hPoints[i].y - 128;
                    }
                }

                if (hPoints[0].y < -128)
                {
                    for (int i = 0; i <= hLineCount; i++)
                    {
                        hPoints[i].y = hPoints[i].y + 128;
                    }
                }

                if (vPoints[0].x > 128)
                {
                    for (int i = 0; i <= vLineCount; i++)
                    {
                        vPoints[i].x = vPoints[i].x - 128;
                    }
                }

                if (vPoints[0].x <= -128)
                {
                    for (int i = 0; i < vLineCount; i++)
                    {
                        vPoints[i].x = vPoints[i].x + 128;
                    }
                }
            }

            if (red > 255)
            {
                red = 0;
            }

            if (green > 255)
            {
                green = 0;
            }

            if (green == 255)
            {
                colorChanger = -1;
            }
            if (green == 0)
            {
                colorChanger = 1;
            }

            if (blue > 255)
            {
                blue = 0;
            }

            if (playerRect.x > windowWidth)
            {
                playerRect.x = 0;
            }
            else if (playerRect.x < 0)
            {
                playerRect.x = windowWidth;
            }

            if (playerRect.y > windowHeight)
            {
                playerRect.y = 0;
            }
            else if (playerRect.y < 0)
            {
                playerRect.y = windowHeight;
            }

            SDL_SetRenderDrawColor(pRenderer, red, green, blue, 255);
            SDL_RenderClear(pRenderer);

            SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
            // SDL_RenderDrawLines(pRenderer, vertPoints, vLineCount);
            // SDL_RenderDrawLines(pRenderer, horiPoints, hLineCount);
            /*
            for (int i = 0; i < vLineCount; i++)
            {
                SDL_RenderDrawLine(pRenderer, vPoints[i].x, vPoints[i].y, vPoints[i + 1].x, vPoints[i + 1].y);
                i++;
            }

            for (int i = 0; i < hLineCount; i++)
            {
                SDL_RenderDrawLine(pRenderer, hPoints[i].x, hPoints[i].y, hPoints[i + 1].x, hPoints[i + 1].y);
                i++;
            }*/
            for (int i = 0; i < nrOfWalls; i++)
            {
                SDL_RenderFillRect(pRenderer, &walls[i].wall);
                // SDL_RenderDrawRect(pRenderer, &walls[i].wall);
            }

            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(pRenderer, &playerRect);
            SDL_RenderCopy(pRenderer, pTexture, NULL, &playerRect);
            SDL_RenderPresent(pRenderer);
        }
    }
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}

/*  READ FROM config.txt FILE AT CONFIG_FILEPATH
    pWindowWidth is a pointer to the int windowWidth
    pWindowHeight is a pointer to the int windowHeight
    TODO: expand in the future
*/
void initConfig(int *pWindowWidth, int *pWindowHeight)
{
    // READ FROM CONFIG:TXT
    FILE *fp;
    fp = fopen(CONFIG_FILEPATH, "r");
    if (fp != NULL)
    {
        while (!feof(fp))
        {
            char configType;
            fscanf(fp, " %c", &configType);
            switch (configType)
            {
            case 'R':
                fscanf(fp, "%dx%d", pWindowWidth, pWindowHeight);
                break;
            default:
                break;
                // case 'N' : fscanf(fp, "%20s", char *windowName);              break;
            }
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR READING FILE");
    }
}

void initMap(int map[][MAPSIZE])
{
    // READ FROM map.txt
    FILE *fp;
    fp = fopen(MAP_FILEPATH, "r");
    if (fp != NULL)
    {
        // while (!feof(fp))
        //{
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
                fscanf(fp, "%d", &map[row][col]);
        }
        //}
        fclose(fp);
    }
    else
    {
        printf("ERROR READING FILE");
    }
}
Wall createWall(int row, int col)
{
    Wall i;
    i.wall.x = col * TILESIZE;
    i.wall.y = row * TILESIZE;
    i.wall.w = TILESIZE;
    i.wall.h = TILESIZE;
    return i;
}