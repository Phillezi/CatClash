#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>

#include "definitions.h"
#include "init.h"

void saveToFile(Tile map[], char fileName[31]);

int main(int argv, char **args)
{
    int updateScreen = 1;
    int prevTime = 0;
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 255, blue = 255;
    Tile map[MAPSIZE * MAPSIZE];

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

    int running = 1;
    char fileName[31];

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
            if (currentKeyStates[SDL_SCANCODE_1])
            {
                if(mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 1;
            }
            else if (currentKeyStates[SDL_SCANCODE_2])
            {
                if(mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 2;
            }
            else if (currentKeyStates[SDL_SCANCODE_3])
            {
                if(mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 3;
            }
            else if (currentKeyStates[SDL_SCANCODE_4])
            {
                if(mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 4;
            }
            else if (currentKeyStates[SDL_SCANCODE_S] && currentKeyStates[SDL_SCANCODE_LCTRL])
            {
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

            if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                if(mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 1;
            }

            if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                if(mouseY < (TILESIZE * MAPSIZE) && mouseX < (TILESIZE * MAPSIZE))
                    map[((mouseY / TILESIZE * MAPSIZE) + (mouseX / TILESIZE))].type = 0;
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
                    //SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[0], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 2:
                    //SDL_SetRenderDrawColor(pRenderer, 100, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[1], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 3:
                    //SDL_SetRenderDrawColor(pRenderer, 0, 100, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[2], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 4:
                    //SDL_SetRenderDrawColor(pRenderer, 0, 0, 100, 255);
                    SDL_RenderCopy(pRenderer, pTextureTiles[3], NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                default:
                    break;
                }
            }
            // Draw grid
            SDL_SetRenderDrawColor(pRenderer, 100, 100, 100, 255);
            for(int line = 0; line < TILESIZE*MAPSIZE; line+=TILESIZE){
                SDL_RenderDrawLine(pRenderer, 0, line, TILESIZE*MAPSIZE, line);
                SDL_RenderDrawLine(pRenderer, line, 0, line, TILESIZE*MAPSIZE);
            }
            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
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
    if(strstr(fileName,".txt"))
        sprintf(location,"%s%s",SAVE_MAP_PATH, fileName);
    else
        sprintf(location,"%s%s.txt", SAVE_MAP_PATH, fileName);
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