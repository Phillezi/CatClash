#ifndef PLAYER_H
#define PLAYER_H

#include "adts.h"

Player *createNewPlayer(Game *pGame, int size);

void destroyPlayer(Player *pPlayers);

void centerPlayer(App *pApp, Player *pPlayer);

void movePlayer(Player *pPlayer, char direction);

int checkCollision(Player player, Tile map[], char direction, int tileSize);

SDL_Rect findEmptyTile(Tile map[]);

SDL_Rect findSpawnTile(Tile map[]);

void getPlayerSpawnPos(Game *pGame);

void translatePositionToScreen(Game *pGame);

#endif