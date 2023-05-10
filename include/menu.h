#include "definitions.h"
#ifndef menu_h
#define menu_h

int menu(Game *pGame);
int mapSelection(Game *pGame);
int joinServerMenu(Game *pGame);
int getStringFromUser(char text[], SDL_Event event);
int testSelectMenu(Game *pGame, char *mapName);
int catSelMenu(Game *pGame);
int mainMenu(Game *pGame);
int serverSelectMenu(Game *pGame);
void render_rounded_rect(SDL_Renderer *pRenderer, SDL_Rect rect, int radius);


#endif