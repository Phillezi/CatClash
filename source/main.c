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
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define WINDOW_NAME "Kitten Game"
#define FPS 165

void initConfig(int *pWindowWidth, int *pWindowHeight);

int main(int argv, char **args)
{

    int prevTime = 0;
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 255, blue = 255;

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

    SDL_Surface *pSurface = IMG_Load("resources/caet2.png");
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

    playerRect.x = 100;
    playerRect.y = 100;

    playerRect.w = 64;
    playerRect.h = 128;

    int running = 1;

    while (running)
    {
        int deltaTime = SDL_GetTicks() - prevTime;
        if (deltaTime >= 1000 / FPS) // updates at a frequency of FPS
        {
            prevTime = SDL_GetTicks();
            // red += 2;
            // green += 10;
            // blue += 5;

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = 0;

                const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);

                if (currentKeyStates[SDL_SCANCODE_W])
                {
                    playerRect.y -= 10;
                }
                if (currentKeyStates[SDL_SCANCODE_S])
                {
                    playerRect.y += 10;
                }
                if (currentKeyStates[SDL_SCANCODE_A])
                {
                    playerRect.x -= 10;
                }
                if (currentKeyStates[SDL_SCANCODE_D])
                {
                    playerRect.x += 10;
                }
                /*
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_w)
                {
                    playerRect.y--;
                }
                if (event.key.keysym.sym == SDLK_s)
                {
                    playerRect.y++;
                }
                if (event.key.keysym.sym == SDLK_a)
                {
                    playerRect.x--;
                }
                if (event.key.keysym.sym == SDLK_d)
                {
                    playerRect.x++;
                }
                */
                /*
                switch (event.key.keysym.sym)
                {
                case SDLK_w: playerRect.y--; break;
                case SDLK_s: playerRect.y++; break;
                case SDLK_a: playerRect.x--; break;
                case SDLK_d: playerRect.x--; break;
                }*/
                //}
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

            if(playerRect.x > windowWidth){
                playerRect.x = 0;
            }
            else if(playerRect.x < 0){
                playerRect.x = windowWidth;
            }

            if(playerRect.y > windowHeight){
                playerRect.y = 0;
            }
            else if(playerRect.y < 0){
                playerRect.y = windowHeight;
            }

            SDL_SetRenderDrawColor(pRenderer, red, green, blue, 255);
            SDL_RenderClear(pRenderer);
            SDL_RenderCopy(pRenderer, pTexture, NULL, &playerRect);
            SDL_RenderPresent(pRenderer);
        }

        // SDL_Delay(1000 / 60);
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