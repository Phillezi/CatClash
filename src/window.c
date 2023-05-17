#include "window.h"

Window *createWindow()
{
    Window *pNew_window = (Window *)malloc(sizeof(Window));
    if(!pNew_window)
    {
        printf("Failed to allocate memory for Window\n");
        return NULL;
    }
    if(initalizeWindow(pNew_window))
    {
        printf("Failed to initialize window\n");
        return NULL;
    }
    return pNew_window;
}

void destroyWindow(Window *pWindow)
{
    if(pWindow->pRenderer)
        SDL_DestroyRenderer(pWindow->pRenderer);
    if(pWindow->pWindow)
        SDL_DestroyWindow(pWindow->pWindow);

    SDL_Quit();
    
    if(pWindow)
        free(pWindow);
}

int initalizeWindow(Window *pWindow)
{
    if(SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
    {
        printf("SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
        return 1;
    }

    pWindow->width = displayMode.w * 0.5;
    pWindow->height = displayMode.h * 0.5;

    pWindow->pWindow = SDL_CreateWindow("Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pWindow->width, pWindow->height, 0);
    if (!pWindow->pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    pWindow->pRenderer = SDL_CreateRenderer(pWindow->pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pWindow->pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    return 0;
}