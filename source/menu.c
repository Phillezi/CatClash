#include "definitions.h"
#include "text.h"
#include "menu.h"
#include "player.h"
#include "init.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int menu(Game *pGame)
{
    int selectedMode = 0, previousTime = 0;
    int r = rand() % 255 + 1, g = rand() % 255 + 1, b = rand() % 255 + 1;
    int rAdd = 1, gAdd = 1, bAdd = 1;
    // Text *pPlay = malloc(sizeof(Text)), *pLvlEdit = malloc(sizeof(Text)), *pQuit = malloc(sizeof(Text));
    Text *pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
    Text *pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
    Text *pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
    Text *pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                freeText(pPlay);
                freeText(pLvlEdit);
                freeText(pQuit);
                freeText(pJoinServer);
                return 2;
            }

            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    if (selectedMode > 0)
                        selectedMode--;
                    else
                        selectedMode = 3;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {

                    if (selectedMode < 3)
                        selectedMode++;
                    else
                        selectedMode = 0;
                }
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    freeText(pPlay);
                    freeText(pLvlEdit);
                    freeText(pQuit);
                    freeText(pJoinServer);
                    return selectedMode;
                }
            }
        }

        if (SDL_GetTicks() - previousTime >= 1000 / 60)
        {
            previousTime = SDL_GetTicks();
            switch (rand() % 3 + 1)
            {
            case 1:
                r += rAdd;
                switch (r)
                {
                case 255:
                    rAdd = -1;
                    break;
                case 0:
                    rAdd = 1;
                    break;
                }

                break;
            case 2:
                g += gAdd;
                switch (g)
                {
                case 255:
                    gAdd = -1;
                    break;
                case 0:
                    gAdd = 1;
                    break;
                }
                break;
            case 3:
                b += bAdd;
                switch (b)
                {
                case 255:
                    bAdd = -1;
                    break;
                case 0:
                    bAdd = 1;
                    break;
                }
                break;
            }
        }
        switch (selectedMode)
        {
        case 0:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            pPlay = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            break;
        case 1:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            break;
        case 2:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            break;
        case 3:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            break;
        }

        SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pGame->pRenderer);
        drawText(pPlay, pGame->pRenderer);
        drawText(pLvlEdit, pGame->pRenderer);
        drawText(pQuit, pGame->pRenderer);
        drawText(pJoinServer, pGame->pRenderer);
        SDL_RenderPresent(pGame->pRenderer);
    }
    freeText(pPlay);
    freeText(pLvlEdit);
    freeText(pQuit);
    freeText(pJoinServer);
    return 0;
}

int mapSelection(Game *pGame)
{
    int previousTime = 0;
    bool exit = false;
    Text *pPrompt = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Type the name", pGame->windowWidth / 2, (pGame->windowHeight / 2) - (2 * pGame->world.tileSize));
    Text *pPrompt2 = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "of the map:", pGame->windowWidth / 2, (pGame->windowHeight / 2) - pGame->world.tileSize);
    Text *pMap = malloc(sizeof(Text));

    int counter = 0;
    char text[31] = {0};
    while (!exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                freeText(pMap);
                freeText(pPrompt);
                freeText(pPrompt2);
                return 1;
            }
            else if (getStringFromUser(text, event))
                if (initMap(pGame->map, text, pGame->world.tileSize))
                {
                    printf("No file found\n");
                }
                else
                {
                    getPlayerSpawnPos(pGame);
                    exit = true;
                }
            else
            {
                if (text[0])
                {
                    freeText(pMap);
                    pMap = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, text, pGame->windowWidth / 2, pGame->windowHeight / 2);
                }
            }
        }
        if (SDL_GetTicks() - previousTime >= 1000 / 60)
        {
            previousTime = SDL_GetTicks();

            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            drawText(pPrompt, pGame->pRenderer);
            drawText(pPrompt2, pGame->pRenderer);
            if (text[0])
            {
                drawText(pMap, pGame->pRenderer);
            }

            SDL_RenderPresent(pGame->pRenderer);
        }
    }
    freeText(pMap);
    freeText(pPrompt);
    freeText(pPrompt2);
    return 0;
}

int joinServerMenu(Game *pGame)
{
    int previousTime = 0;
    bool exit = false;
    Text *pPrompt = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Type the IP", pGame->windowWidth / 2, (pGame->windowHeight / 2) - (2 * pGame->world.tileSize));
    Text *pPrompt2 = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "of the Server:", pGame->windowWidth / 2, (pGame->windowHeight / 2) - pGame->world.tileSize);
    Text *pIpText = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "192.168.0.1:1234", pGame->windowWidth / 2, (pGame->windowHeight / 2));

    int counter = 0;
    char text[31] = {0};
    while (!exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                freeText(pIpText);
                freeText(pPrompt);
                freeText(pPrompt2);
                return 1;
            }
            else if (getStringFromUser(text, event))
            {
                exit = true;
                /*
                if (!tryConnectTcp(text))
                {
                    close(pGame);
                    clientTCP(text);
                    exit = true;
                }
                */

                /*
                // NET INIT
                if (!(pGame->socketDesc = SDLNet_UDP_Open(0)))
                {
                    fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
                    exit(EXIT_FAILURE);
                }
                if (SDLNet_ResolveHost(&pGame->serverAddress, text, 1234))
                {
                    fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                    exit(EXIT_FAILURE);
                }
                if (!(pGame->pPacket = SDLNet_AllocPacket(512)))
                {
                    fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
                    exit(EXIT_FAILURE);
                }
                pGame->nrOfPlayers++;
                pGame->pPacket->address.host = pGame->serverAddress.host;
                pGame->pPacket->address.port = pGame->serverAddress.port;
                */
            }

            else
            {
                if (text[0])
                {
                    freeText(pIpText);
                    pIpText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, text, pGame->windowWidth / 2, pGame->windowHeight / 2);
                }
            }
        }
        if (SDL_GetTicks() - previousTime >= 1000 / 60)
        {
            previousTime = SDL_GetTicks();

            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            drawText(pPrompt, pGame->pRenderer);
            drawText(pPrompt2, pGame->pRenderer);
            if (text[0])
            {
                drawText(pIpText, pGame->pRenderer);
            }

            SDL_RenderPresent(pGame->pRenderer);
        }
    }
    freeText(pIpText);
    freeText(pPrompt);
    freeText(pPrompt2);
    return 0;
}

int getStringFromUser(char text[], SDL_Event event)
{
    int strCounter = strlen(text), strEnd = 0;
    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_RETURN)
        {
            if (strCounter)
            {
                strEnd = 1;
            }
        }
        if (event.key.keysym.sym == SDLK_BACKSPACE)
        {
            if (strCounter > 0)
            {
                text[strCounter - 1] = 0;
                strCounter--;
            }
        }
        else
        {
            if ((event.key.keysym.sym >= 'a' && event.key.keysym.sym <= '~') || (event.key.keysym.sym >= ' ' && event.key.keysym.sym <= '9'))
            {

                if (strCounter < 31)
                {
                    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
                    if (((event.key.keysym.sym > 96 && event.key.keysym.sym < 127)) && (currentKeyStates[SDL_SCANCODE_LSHIFT] || currentKeyStates[SDL_SCANCODE_RSHIFT]))
                        text[strCounter] = event.key.keysym.sym - 32;
                    else if (event.key.keysym.sym == '.' && (currentKeyStates[SDL_SCANCODE_LSHIFT] || currentKeyStates[SDL_SCANCODE_RSHIFT]))
                        text[strCounter] = ':';
                    else
                    {
                        text[strCounter] = event.key.keysym.sym;
                    }

                    strCounter++;
                }
            }
        }
    }
    return strEnd;
}