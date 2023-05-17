#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL_ttf.h>
#include "text.h"
#include "adts.h"

#define FONT_PATH "resources/fonts/RetroGaming.ttf"

Menu *createMenu(void *pAppIn);

void destroyMenu(Menu *pMenu);

int initalizeMenu(Menu *pMenu, void *pAppIn);

#endif