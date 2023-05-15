#include "definitions.h"
#ifndef menu_h
#define menu_h

int joinServerMenu(Game *pGame);
int getStringFromUser(char text[], SDL_Event event);
int testSelectMenu(Game *pGame, char *mapName);
int catSelMenu(Game *pGame);
int mainMenu(Game *pGame);
int serverSelectMenu(Game *pGame);
int serverLobby(Game *pGame);


#endif