#include "definitions.h"
#ifndef player_h
#define player_h

void centerPlayer(Game *pGame);
int handleInput(Game *pGame);
void movePlayer(Player *pPlayer, char direction);
int checkCollision(Player player, Tile map[], char direction, int tileSize);
SDL_Rect findEmptyTile(Tile map[]);
void getPlayerSpawnPos(Game *pGame);

#endif