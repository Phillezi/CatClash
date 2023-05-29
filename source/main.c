#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "../include/definitions.h"
#include "../include/game.h"
#include "../include/init.h"
#include "../include/text.h"
#include "../include/menu.h"
#include "../include/player.h"
#include "../include/levelEditor.h"
#include "../include/newClient.h"

int main(int argv, char **args)
{
    printf("If the game doesnt start, try again or comment out \"#include <SDL2/SDL_mixer.h>\" in \"\\include\\definitions.h\"\n");
    Game *pGame = createGame();
    char *mapName = (char *)malloc(31*sizeof(char));

    if(!pGame)
    {
        printf("Couldnt create Game\n");
        free(mapName);
        return 1;
    }
    if (initGame(pGame))
    {
        free(mapName);
        closeGame(pGame);
        return 1;
    }
    while (1)
    {
        switch (mainMenu(pGame))
        {
        case PLAY:
            if (testSelectMenu(pGame, mapName))
                break;
            runGame(pGame);
            break;
        case EDIT:
            if (testSelectMenu(pGame, mapName))
                break;
            levelEditor(pGame);
            break;
        case QUIT:
            printf("Closing game...\n");
            closeGame(pGame);
            printf("Done closing!\n");
            free(mapName);
            return 0;
            break;
        case JOIN:
            switch (serverSelectMenu(pGame))
            {
            case 0:
                break;
            case 1:
                if (joinServerMenu(pGame))
                    break;
            case 2:
                while (!serverLobby(pGame))
                {
                    runGame(pGame);
                }
                break;
            }
            break;
        case CATSEL:
            if (catSelMenu(pGame))
                break;
            break;
        case HOST:
            break;
            break;
        default:
            break;
        }
    }
    closeGame(pGame);
    free(mapName);
    return 0;
}