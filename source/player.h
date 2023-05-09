#include "definitions.h"
#ifndef player_h
#define player_h

void centerPlayer(Game *pGame, Player *pPlayer);
void *handleInput(void *pGameIn);//Game *pGame);
void movePlayer(Player *pPlayer, char direction);
int checkCollision(Player player, Tile map[], char direction, int tileSize);
int playerCollision(Player player, Player players[], int nrOfPlayers, char direction, int tileSize, int extraLength);
SDL_Rect findEmptyTile(Tile map[]);
SDL_Rect findSpawnTile(Tile map[]);
void getPlayerSpawnPos(Game *pGame);
Player *createPlayer(int id, char *name, int tileSize);
void destroyPlayer(Player *pPlayer);
void translatePositionToScreen(Game *pGame);
int changePlayerTexture(SDL_Renderer *pRenderer, SDL_Window *pWindow, SDL_Texture **pTexturePlayer, char direction);
void loadMedia(SDL_Renderer *pRenderer, SDL_Texture **pPlayerTexture, SDL_Rect gSpriteClips[][29], int playerID);
void drawPlayer(Game *pGame, Player player, int i);
Player *createNewMultiPlayer(Game *pGame, int size, Player data);
Player *removePlayer(Game *pGame, int size);
int getAlivePlayers(Game *pGame);
int getDeadPlayers(Game *pGame);
void checkChargingPlayers(Game *pGame);
int checkDirectionsOtherThanFront(Game *pGame);

#endif