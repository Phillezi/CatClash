#include "engine.h"
#include <SDL2/SDL.h>

/*  NOT DONE
    Initialize a new window
    Expected inputs: name string, int window width, int window height
    Returns the adress of pWindow
*/
int initWindow(char windowName[], int windowWidth, int windowHeight){
    SDL_Window *pWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    if (!pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    return pWindow;
}
/*  NOT DONE
    Initialize a new renderer
    Expected inputs: pWindow
    Returns the adress of pRenderer
*/
int initRenderer(SDL_Window pWindow){
    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    return pRenderer;
}

int delWindow(){
    
}