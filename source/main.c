/*
    KITTENFORK main.c HI1038
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include "engine.h"

// Network
#include <SDL2/SDL_net.h>
#include "client.h"
#include "server.h"

#define CONFIG_FILEPATH "resources/config.txt"
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define WINDOW_NAME "Kitten Game"

void initConfig(int *pWindowWidth, int *pWindowHeight);

int main(int argv, char **args)
{
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 0, blue = 0;

    initConfig(&windowWidth, &windowHeight);

    printf("Your current resolution is %dx%d", windowWidth, windowHeight);

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

    int running = 1;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w: red++; break;
                case SDLK_d: green++; break;
                case SDLK_a: blue++; break;
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

            if (blue > 255)
            {
                blue = 0;
            }
            
        }
        SDL_SetRenderDrawColor(pRenderer, red, green, blue, 255);
        SDL_RenderClear(pRenderer);
        SDL_RenderPresent(pRenderer);
        SDL_Delay(1000 / 60);
    }

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