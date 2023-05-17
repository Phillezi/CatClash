#include "player.h"

Player *createNewPlayer(Game *pGame, int size)
{
    Player *pNew_arr;
    if (pGame->pPlayers)
    {
        printf("Expanding memory for player array\n");
        pNew_arr = (Player *)realloc(pGame->pPlayers, (size + 1) * sizeof(Player));
    }
    else
    {
        pNew_arr = (Player *)malloc((size + 1) * sizeof(Player));
    }

    if (pNew_arr == NULL)
    {
        printf("ERROR when allocating new memory for joined player\n");
        return NULL;
    }

    return pNew_arr;
}

Player *removePlayer(App *pApp, int size)
{
    if (pApp->pGame->pPlayers == NULL)
    {
        printf("ERROR: Player array is empty\n");
        return NULL;
    }

    printf("Shrinking memory for player array\n");
    Player *pNew_arr = (Player *)realloc(pApp->pGame->pPlayers, size * sizeof(Player));
    if (pNew_arr == NULL && size > 0)
    {
        printf("ERROR when reallocating memory for player array\n");
        return NULL;
    }


    pApp->pGame->pPlayers = pNew_arr;
    return pNew_arr;
}

void destroyPlayer(Player *pPlayers)
{
    free(pPlayers);
}