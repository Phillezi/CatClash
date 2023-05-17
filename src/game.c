#include "game.h"

Game *createGame()
{
    Game *pNew_game = (Game *)malloc(sizeof(Game));
    if (!pNew_game)
    {
        printf("Failed to allocate memory for game\n");
        return NULL;
    }
    if (initalizegame(pNew_game))
    {
        printf("Failed to initialize game\n");
        return NULL;
    }
    return pNew_game;
}

void destroyGame(Game *pGame)
{
    if(pGame)
        free(pGame);
}

int initalizegame(Game *pNew_game)
{

    return 0;
}