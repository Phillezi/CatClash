#include "menu.h"
#include <stdio.h>

int menu(SDL_Window *pWindow, SDL_Renderer *pRenderer)
{
    TTF_Init();

    int running = 1, prevTime = 0;
    int oneSecTimer = 0, frameCounter = 0;

    SDL_Rect test;
    test.w = DEFAULT_WIDTH - 20;
    test.h = DEFAULT_HEIGHT / 2;
    test.x = 10;
    test.y = 20;

    SDL_Rect fpsFrame;
    fpsFrame.w = 40;
    fpsFrame.h = 40;
    fpsFrame.x = DEFAULT_WIDTH - fpsFrame.w;
    fpsFrame.y = 0;

    TTF_Font *pFont = TTF_OpenFont("resources/fonts/Pixelletters.ttf", 20);
    SDL_Color color = {255, 255, 255};
    SDL_Surface *pSurface = TTF_RenderText_Solid(pFont, "Press SPACE to start", color);
    SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);

    SDL_Color green = {0, 255, 0};
    SDL_Surface *pFpsSurface = TTF_RenderText_Solid(pFont, "FPS", green);
    SDL_Texture *pFpsTexture = SDL_CreateTextureFromSurface(pRenderer, pFpsSurface);
    SDL_FreeSurface(pFpsSurface);

    while (running)
    {
        if (SDL_GetTicks() - oneSecTimer >= 1000) // Performance monitor
        {
            oneSecTimer = SDL_GetTicks();
            // printf("FPS: %d\n", frameCounter);
            char buffer[50];
            SDL_DestroyTexture(pFpsTexture);
            sprintf(buffer, "%d", frameCounter);
            SDL_Surface *pFpsSurface = TTF_RenderText_Solid(pFont, buffer, green);
            SDL_Texture *pFpsTexture = SDL_CreateTextureFromSurface(pRenderer, pFpsSurface);
            SDL_FreeSurface(pFpsSurface);
            frameCounter = 0;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;
        }
        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_SPACE])
        {
            return 0;
        }
        int deltaTime = SDL_GetTicks() - prevTime;
        if (deltaTime >= 1000 / FPS) // updates at a frequency of FPS
        {
            prevTime = SDL_GetTicks();
            SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pRenderer);
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
            // SDL_SetRenderDrawColor(pRenderer, r, g, b, 255);
            SDL_RenderFillRect(pRenderer, &test);

            SDL_RenderCopy(pRenderer, pTexture, NULL, &test);

            //SDL_RenderFillRect(pRenderer, &fpsFrame);
            SDL_RenderCopy(pRenderer, pFpsTexture, NULL, &fpsFrame);
            SDL_RenderPresent(pRenderer);
            frameCounter++;
        }
    }
    SDL_DestroyTexture(pTexture);
    SDL_DestroyTexture(pFpsTexture);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    TTF_CloseFont(pFont);
    TTF_Quit();
    SDL_Quit();
    return 0;
}