#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include "definitions.h"
#include "init.h"

void saveToFile(Tile map[], char fileName[31]);
int popUp();

int main(int argv, char **args)
{
    int updateScreen = 1;
    int prevTime = 0;
    int windowWidth = DEFAULT_WIDTH + 100, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 255, blue = 255;
    Tile map[MAPSIZE * MAPSIZE];
    char fileName[31];
    char location[100];
    do
    {
        printf("map name?\n: ");
        scanf(" %30s", fileName);

        if (strstr(fileName, ".txt"))
            sprintf(location, "%s%s", SAVE_MAP_PATH, fileName);
        else
            sprintf(location, "%s%s.txt", SAVE_MAP_PATH, fileName);

    } while (initMap(map, location) == -1);

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

    SDL_Rect saveButton;
    saveButton.x = (TILESIZE * MAPSIZE);
    saveButton.y = 0;
    saveButton.w = windowWidth - (TILESIZE * MAPSIZE);
    saveButton.h = windowWidth - (TILESIZE * MAPSIZE);

    SDL_Rect openButton;
    openButton.x = (TILESIZE * MAPSIZE);
    openButton.y = saveButton.h;
    openButton.w = windowWidth - (TILESIZE * MAPSIZE);
    openButton.h = windowWidth - (TILESIZE * MAPSIZE);

    int running = 1;

    while (running)
    {
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

            int mouseX, mouseY;
            int buttons = SDL_GetMouseState(&mouseX, &mouseY);

            const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
            if (currentKeyStates[SDL_SCANCODE_0])
            {
                if (mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 0;
            }
            if (currentKeyStates[SDL_SCANCODE_1])
            {
                if (mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 1;
            }
            else if (currentKeyStates[SDL_SCANCODE_2])
            {
                if (mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 2;
            }
            else if (currentKeyStates[SDL_SCANCODE_3])
            {
                if (mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 3;
            }
            else if (currentKeyStates[SDL_SCANCODE_4])
            {
                if (mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 4;
            }
            else if (currentKeyStates[SDL_SCANCODE_S] && currentKeyStates[SDL_SCANCODE_LCTRL])
            {
                // popUp();
                printf("What would you like to name the file?\n: ");
                scanf(" %30s", fileName);
                saveToFile(map, fileName);
                printf("saved file");
            }
            else if (currentKeyStates[SDL_SCANCODE_DELETE])
            {
                for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                {
                    map[i].type = 0;
                }
            }
            else if (currentKeyStates[SDL_SCANCODE_INSERT])
            {
                for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                {
                    map[i].type = 1;
                }
            }
            else if (currentKeyStates[SDL_SCANCODE_W])
            {
                if (map[0].wall.w >= 5)
                {
                    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                    {
                        map[i].wall.w--;
                        map[i].wall.h--;
                        map[i].wall.x -= (i % (MAPSIZE));
                        map[i].wall.y -= (i / (MAPSIZE));
                    }
                }
            }
            else if (currentKeyStates[SDL_SCANCODE_S])
            {
                if (map[0].wall.w < windowWidth)
                {
                    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                    {
                        map[i].wall.w++;
                        map[i].wall.h++;
                        map[i].wall.x += (i % (MAPSIZE));
                        map[i].wall.y += (i / (MAPSIZE));
                    }
                }
            }
            else if (currentKeyStates[SDL_SCANCODE_D])
            {
                for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                {
                    // map[i].wall.x++;
                }
            }
            else if (currentKeyStates[SDL_SCANCODE_A])
            {
                for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                {
                    // map[i].wall.x--;
                }
            }

            //float scale = ((float)map[0].wall.w / TILESIZE);

            if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                if (mouseY < (map[0].wall.w * MAPSIZE) && mouseX < (map[0].wall.w * MAPSIZE))
                    map[((mouseY / map[0].wall.w * MAPSIZE) + (mouseX / map[0].wall.w))].type = 1;
            }

            if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                if (mouseY < (map[0].wall.w * MAPSIZE) && mouseX < (map[0].wall.w * MAPSIZE))
                    map[((mouseY / map[0].wall.w * MAPSIZE) + (mouseX / map[0].wall.w))].type = 0;
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
            // Draw grid
            SDL_SetRenderDrawColor(pRenderer, 100, 100, 100, 255);
            for (int line = 0; line < map[0].wall.w * MAPSIZE; line += map[0].wall.w)
            {
                SDL_RenderDrawLine(pRenderer, map[0].wall.x, line, map[MAPSIZE - 1].wall.x + map[0].wall.w, line);
                SDL_RenderDrawLine(pRenderer, line, map[0].wall.y, line, map[MAPSIZE * MAPSIZE - 1].wall.y + map[0].wall.w);
            }

            if ((mouseY < (map[0].wall.w * MAPSIZE)) && (mouseX < (map[0].wall.w * MAPSIZE)))
            {
                SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
                SDL_RenderDrawRect(pRenderer, &map[(((mouseY) / map[0].wall.w * MAPSIZE) + ((mouseX) / map[0].wall.w))].wall);
            }

            SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
            SDL_RenderFillRect(pRenderer, &saveButton);

            SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
            SDL_RenderFillRect(pRenderer, &openButton);

            SDL_RenderPresent(pRenderer);
        }
    }
    SDL_DestroyTexture(pTextureTiles[0]);
    SDL_DestroyTexture(pTextureTiles[1]);
    SDL_DestroyTexture(pTextureTiles[2]);
    SDL_DestroyTexture(pTextureTiles[3]);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}

void saveToFile(Tile map[], char fileName[31])
{
    char location[100];
    if (strstr(fileName, ".txt"))
        sprintf(location, "%s%s", SAVE_MAP_PATH, fileName);
    else
        sprintf(location, "%s%s.txt", SAVE_MAP_PATH, fileName);
    printf("Saved at: %s\n", location);
    FILE *fp;
    fp = fopen(location, "w+");
    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                fprintf(fp, "%d ", map[row * MAPSIZE + col].type);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR opening FILE");
    }
}
int popUp()
{
    SDL_Window *pWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 400, 0);
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

    int prevTime = 0, running = 1;
    while (running)
    {
        int deltaTime = SDL_GetTicks() - prevTime;
        if (deltaTime >= 1000 / 60)
        {
            prevTime = SDL_GetTicks();
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = 0;
            }
            const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
            if (currentKeyStates[SDL_SCANCODE_KP_ENTER])
            {
                running = 0;
            }
            SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pRenderer);
            SDL_RenderPresent(pRenderer);
        }
    }

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    return 0;
}