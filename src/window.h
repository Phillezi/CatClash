#ifndef WINDOW_H
#define WINDOW_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

typedef struct {
    Uint32 width, height;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    
} Window;

Window *createWindow();

void destroyWindow(Window *pWindow);

int initalizeWindow(Window *pWindow);

#endif
