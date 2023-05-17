#ifndef WINDOW_H
#define WINDOW_H

#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "adts.h"

Window *createWindow();

void destroyWindow(Window *pWindow);

int initalizeWindow(Window *pWindow);

#endif
