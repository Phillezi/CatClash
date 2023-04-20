#include "definitions.h"
#ifndef menu_h
#define menu_h

int menu(Game *pGame);
int mapSelection(Game *pGame);
int joinServerMenu(Game *pGame);
int getStringFromUser(char text[], SDL_Event event);

#endif