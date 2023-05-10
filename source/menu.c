#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../include/ioHandler.h"
#include "../include/newClient.h"
#include "../include/definitions.h"
#include "../include/text.h"
#include "../include/menu.h"
#include "../include/player.h"
#include "../include/init.h"

int menu(Game *pGame)
{
    int selectedMode = 0, previousTime = 0;
    int r = rand() % 255 + 1, g = rand() % 255 + 1, b = rand() % 255 + 1;
    int rAdd = 1, gAdd = 1, bAdd = 1;
    int playW, levelEditW, quitW, joinServerW, catSelectW, hostW;
    // Text *pPlay = malloc(sizeof(Text)), *pLvlEdit = malloc(sizeof(Text)), *pQuit = malloc(sizeof(Text));
    Text *pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
    TTF_SizeText(pGame->ui.pFpsFont, "Play", &playW, NULL);
    Text *pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
    TTF_SizeText(pGame->ui.pFpsFont, "Edit level", &levelEditW, NULL);
    Text *pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
    TTF_SizeText(pGame->ui.pFpsFont, "QUIT", &quitW, NULL);
    Text *pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
    TTF_SizeText(pGame->ui.pFpsFont, "Join Server", &joinServerW, NULL);
    Text *pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
    TTF_SizeText(pGame->ui.pFpsFont, "Cat Selection", &catSelectW, NULL);
    Text *pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
    TTF_SizeText(pGame->ui.pFpsFont, "Host Server", &hostW, NULL);
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
                freeText(pCatSelect);
                freeText(pHost);
                return 2;
            }
            else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
            {

                if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
                {
                    freeText(pPlay);
                    freeText(pLvlEdit);
                    freeText(pQuit);
                    freeText(pJoinServer);
                    freeText(pCatSelect);
                    freeText(pHost);
                    return 2;
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    if (selectedMode > 0)
                        selectedMode--;
                    else
                        selectedMode = 5;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {

                    if (selectedMode < 5)
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
                    freeText(pCatSelect);
                    freeText(pHost);
                    return selectedMode;
                }
            }
            else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX, mouseY;
                int buttons = SDL_GetMouseState(&mouseX, &mouseY);
                int centerOfScreenX = pGame->windowWidth / 2;
                int topOfFirstRowY = pGame->windowHeight / 4;
                int topOfSecondRowY = (pGame->windowHeight / 4) + pGame->world.tileSize;
                int topOfThirdRowY = (pGame->windowHeight / 4) + (2 * pGame->world.tileSize);
                int topOfFourthRowY = (pGame->windowHeight / 4) + (3 * pGame->world.tileSize);
                int topOfFifthRowY = (pGame->windowHeight / 4) + (4 * pGame->world.tileSize);
                int topOfSixthRowY = (pGame->windowHeight / 4) + (5 * pGame->world.tileSize);
                if (centerOfScreenX - (playW / 2) < mouseX && mouseX < centerOfScreenX + (playW / 2) && topOfFirstRowY - (pGame->world.tileSize / 2) < mouseY && mouseY < topOfFirstRowY + (pGame->world.tileSize / 2))
                {
                    selectedMode = 0;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        freeText(pPlay);
                        freeText(pLvlEdit);
                        freeText(pQuit);
                        freeText(pJoinServer);
                        freeText(pCatSelect);
                        freeText(pHost);
                        return selectedMode;
                    }
                }

                else if (centerOfScreenX - (levelEditW / 2) < mouseX && mouseX < centerOfScreenX + (levelEditW / 2) && topOfSecondRowY - (pGame->world.tileSize / 2) < mouseY && mouseY < topOfSecondRowY + (pGame->world.tileSize / 2))
                {
                    selectedMode = 1;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        freeText(pPlay);
                        freeText(pLvlEdit);
                        freeText(pQuit);
                        freeText(pJoinServer);
                        freeText(pCatSelect);
                        freeText(pHost);
                        return selectedMode;
                    }
                }
                else if (centerOfScreenX - (quitW / 2) < mouseX && mouseX < centerOfScreenX + (quitW / 2) && topOfThirdRowY - (pGame->world.tileSize / 2) < mouseY && mouseY < topOfThirdRowY + (pGame->world.tileSize / 2))
                {
                    selectedMode = 2;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        freeText(pPlay);
                        freeText(pLvlEdit);
                        freeText(pQuit);
                        freeText(pJoinServer);
                        freeText(pCatSelect);
                        freeText(pHost);
                        return selectedMode;
                    }
                }
                else if (centerOfScreenX - (joinServerW / 2) < mouseX && mouseX < centerOfScreenX + (joinServerW / 2) && topOfFourthRowY - (pGame->world.tileSize / 2) < mouseY && mouseY < topOfFourthRowY + (pGame->world.tileSize / 2))
                {
                    selectedMode = 3;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        freeText(pPlay);
                        freeText(pLvlEdit);
                        freeText(pQuit);
                        freeText(pJoinServer);
                        freeText(pCatSelect);
                        freeText(pHost);
                        return selectedMode;
                    }
                }
                else if (centerOfScreenX - (catSelectW / 2) < mouseX && mouseX < centerOfScreenX + (catSelectW / 2) && topOfFifthRowY - (pGame->world.tileSize / 2) < mouseY && mouseY < topOfFifthRowY + (pGame->world.tileSize / 2))
                {
                    selectedMode = 4;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        freeText(pPlay);
                        freeText(pLvlEdit);
                        freeText(pQuit);
                        freeText(pJoinServer);
                        freeText(pCatSelect);
                        freeText(pHost);
                        return selectedMode;
                    }
                }
                else if (centerOfScreenX - (hostW / 2) < mouseX && mouseX < centerOfScreenX + (hostW / 2) && topOfSixthRowY - (pGame->world.tileSize / 2) < mouseY && mouseY < topOfSixthRowY + (pGame->world.tileSize / 2))
                {
                    selectedMode = 5;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        freeText(pPlay);
                        freeText(pLvlEdit);
                        freeText(pQuit);
                        freeText(pJoinServer);
                        freeText(pCatSelect);
                        freeText(pHost);
                        return selectedMode;
                    }
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
            freeText(pCatSelect);
            freeText(pHost);
            pPlay = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
            pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
            break;
        case 1:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            freeText(pCatSelect);
            freeText(pHost);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
            pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
            break;
        case 2:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            freeText(pCatSelect);
            freeText(pHost);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
            pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
            break;
        case 3:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            freeText(pCatSelect);
            freeText(pHost);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
            pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
            break;
        case 4:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            freeText(pCatSelect);
            freeText(pHost);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            pCatSelect = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
            pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
            break;
        case 5:
            freeText(pPlay);
            freeText(pLvlEdit);
            freeText(pQuit);
            freeText(pJoinServer);
            freeText(pCatSelect);
            freeText(pHost);
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (3 * pGame->world.tileSize));
            pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (4 * pGame->world.tileSize));
            pHost = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Host Server", pGame->windowHeight / 2, (pGame->windowHeight / 4) + (5 * pGame->world.tileSize));
            break;
        }

        SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pGame->pRenderer);
        drawText(pPlay, pGame->pRenderer);
        drawText(pLvlEdit, pGame->pRenderer);
        drawText(pQuit, pGame->pRenderer);
        drawText(pJoinServer, pGame->pRenderer);
        drawText(pCatSelect, pGame->pRenderer);
        drawText(pHost, pGame->pRenderer);
        SDL_RenderPresent(pGame->pRenderer);
    }
    freeText(pPlay);
    freeText(pLvlEdit);
    freeText(pQuit);
    freeText(pJoinServer);
    freeText(pCatSelect);
    freeText(pHost);
    return 0;
}

int mapSelection(Game *pGame)
{
    int previousTime = 0;
    bool exit = false;
    Text *pPrompt = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Type the name", pGame->windowWidth / 2, (pGame->windowHeight / 2) - (2 * pGame->world.tileSize));
    Text *pPrompt2 = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "of the map:", pGame->windowWidth / 2, (pGame->windowHeight / 2) - pGame->world.tileSize);
    Text *pMap = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, ":", pGame->windowWidth / 2, pGame->windowHeight / 2);

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
                pGame->pClient = createClient(text, 1234, 0, 100, 100);
                if (connectToServer(pGame))
                {
                    printf("Error: Could not join server\n");
                    exit = true;
                    free(pGame->pClient);
                    freeText(pIpText);
                    freeText(pPrompt);
                    freeText(pPrompt2);
                    return 1;
                }
                printf("Connected to Server\n");
                initMapFromTCP(pGame->map, pGame->world.tileSize);
                getPlayerSpawnPos(pGame);
                SDLNet_ResolveHost(&pGame->serverAddress, text, 1234);
                printf("Resolved UDP host\n");
                exit = true;
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

int testSelectMenu(Game *pGame, char *mapName)
{
    int len = 0;
    int selected = 0;
    TTF_Font *listFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pGame->world.tileSize / 2);
    char **strArr = checkFolderAndReturnList(SAVE_MAP_PATH, &len);
    Text *pText[len];
    for (int i = 0; i < len; i++)
    {
        if (!(i == selected))
            pText[i] = createText(pGame->pRenderer, 100, 100, 100, listFont, strArr[i], pGame->windowWidth / 2, (pGame->windowHeight / 2) + ((i - selected) * pGame->world.tileSize / 2));
        else
        {
            pText[i] = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, strArr[i], pGame->windowWidth / 2, (pGame->windowHeight / 2) + ((i - selected) * pGame->world.tileSize / 2));
        }
    }
    initMap(pGame->map, strArr[selected], pGame->world.tileSize / 10);

    bool exit = false;
    int previousTime = 0;
    while (!exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                TTF_CloseFont(listFont);
                for (int i = 0; i < len; i++)
                    freeText(pText[i]);
                freeTextList(strArr, len);
                return 1;
                break;
            }

            else if (event.type == SDL_MOUSEWHEEL || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.wheel.y < 0 && event.type == SDL_MOUSEWHEEL || event.key.keysym.sym == SDLK_DOWN) // scroll up
                {
                    if (selected < len - 1)
                    {
                        selected++;
                        for (int i = 0; i < len; i++)
                        {
                            freeText(pText[i]);
                            if (!(i == selected))
                                pText[i] = createText(pGame->pRenderer, 100, 100, 100, listFont, strArr[i], pGame->windowWidth / 2, (pGame->windowHeight / 2) + ((i - selected) * pGame->world.tileSize / 2));
                            else
                            {
                                pText[i] = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, strArr[i], pGame->windowWidth / 2, (pGame->windowHeight / 2) + ((i - selected) * pGame->world.tileSize / 2));
                            }
                        }
                        initMap(pGame->map, strArr[selected], pGame->world.tileSize / 10);
                    }
                }
                else if (event.wheel.y > 0 && event.type == SDL_MOUSEWHEEL || event.key.keysym.sym == SDLK_UP) // scroll down
                {
                    if (selected > 0)
                    {
                        selected--;
                        for (int i = 0; i < len; i++)
                        {
                            freeText(pText[i]);
                            if (!(i == selected))
                                pText[i] = createText(pGame->pRenderer, 100, 100, 100, listFont, strArr[i], pGame->windowWidth / 2, (pGame->windowHeight / 2) + ((i - selected) * pGame->world.tileSize / 2));
                            else
                            {
                                pText[i] = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, strArr[i], pGame->windowWidth / 2, (pGame->windowHeight / 2) + ((i - selected) * pGame->world.tileSize / 2));
                            }
                        }
                        initMap(pGame->map, strArr[selected], pGame->world.tileSize / 10);
                    }
                }
                else if (event.key.keysym.sym == SDLK_RETURN || event.button.button == SDL_BUTTON_LEFT)
                {
                    if (initMap(pGame->map, strArr[selected], pGame->world.tileSize))
                    {
                        printf("No file found\n");
                    }
                    else
                    {
                        getPlayerSpawnPos(pGame);
                        exit = true;
                        break;
                    }
                }
            }
        }
        if (SDL_GetTicks() - previousTime >= 1000 / 60)
        {
            previousTime = SDL_GetTicks();

            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                if (pGame->map[i].type > 0 && pGame->map[i].type < TILES)
                    SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[pGame->map[i].type], NULL, &pGame->map[i].wall);
            }
            for (int i = 0; i < len; i++)
            {
                if (i != selected)
                    drawText(pText[i], pGame->pRenderer);
            }
            drawText(pText[selected], pGame->pRenderer);

            SDL_RenderPresent(pGame->pRenderer);
        }
    }
    TTF_CloseFont(listFont);
    for (int i = 0; i < len; i++)
        freeText(pText[i]);
    freeTextList(strArr, len);
    strcpy(mapName, strArr[selected]);
    return 0;
}

int catSelMenu(Game *pGame)
{
    int previousTime = 0;
    bool exit = false;
    Text *pSelPrompt = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "COMING SOON!", pGame->windowWidth / 2, (pGame->windowHeight / 2));

    while (!exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
            }
        }
        if (SDL_GetTicks() - previousTime >= 1000 / 60)
        {
            previousTime = SDL_GetTicks();
            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            drawText(pSelPrompt, pGame->pRenderer);
            SDL_RenderPresent(pGame->pRenderer);
        }
    }

    freeText(pSelPrompt);
    return 0;
}

int mainMenu(Game *pGame)
{
    SelectedMode mode = PLAY;

    bool quit = false;
    int deltaTime = 0, previousTime = 0;

    int r = rand() % 255 + 1, g = rand() % 255 + 1, b = rand() % 255 + 1;
    int rAdd = 1, gAdd = 1, bAdd = 1;
    int playW, levelEditW, quitW, joinServerW, catSelectW, hostW;

    int centerOfScreenX = pGame->windowWidth / 2;
    int playY = (pGame->windowHeight / 4) + (PLAY * pGame->world.tileSize);
    int editY = (pGame->windowHeight / 4) + (EDIT * pGame->world.tileSize);
    int quitY = (pGame->windowHeight / 4) + (QUIT * pGame->world.tileSize);
    int joinY = (pGame->windowHeight / 4) + (JOIN * pGame->world.tileSize);
    int catselY = (pGame->windowHeight / 4) + (CATSEL * pGame->world.tileSize);
    int hostY = (pGame->windowHeight / 4) + (HOST * pGame->world.tileSize);

    Text *pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", centerOfScreenX, playY);
    Text *pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", centerOfScreenX, editY);
    Text *pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", centerOfScreenX, quitY);
    Text *pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", centerOfScreenX, joinY);
    Text *pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", centerOfScreenX, catselY);
    Text *pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", centerOfScreenX, hostY);

    TTF_SizeText(pGame->ui.pFpsFont, "Play", &playW, NULL);
    TTF_SizeText(pGame->ui.pFpsFont, "Edit level", &levelEditW, NULL);
    TTF_SizeText(pGame->ui.pFpsFont, "QUIT", &quitW, NULL);
    TTF_SizeText(pGame->ui.pFpsFont, "Join Server", &joinServerW, NULL);
    TTF_SizeText(pGame->ui.pFpsFont, "Cat Selection", &catSelectW, NULL);
    TTF_SizeText(pGame->ui.pFpsFont, "Host Server", &hostW, NULL);

    // PLAYERNAME TEST ADDITION---------------------------------------------------------------------------------------------------------------------------
    int playerNameW, playerNameH, previousSecond = 0;
    TTF_SizeText(pGame->ui.pFpsFont, pGame->pPlayer->name, &playerNameW, &playerNameH);
    Text *pName = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, pGame->pPlayer->name, playerNameW / 2, pGame->windowHeight - (playerNameH / 2));
    bool editPlayerName = false, drawLine = false, updatePlayerNameFlag = false;
    //----------------------------------------------------------------------------------------------------------------------------------------------------

    while (!quit)
    {
        int previousMode = mode;
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                mode = QUIT;
                quit = true;
            }

            else if (editPlayerName)
            {
                // EDIT PLAYERNAME ADDITION
                if (getStringFromUser(pGame->pPlayer->name, event))
                {
                    editPlayerName = false;
                    updatePlayerNameFlag = false;
                    TTF_SizeText(pGame->ui.pFpsFont, pGame->pPlayer->name, &playerNameW, &playerNameH);
                    freeText(pName);
                    pName = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, pGame->pPlayer->name, playerNameW / 2, pGame->windowHeight - (playerNameH / 2));
                }
                else
                    updatePlayerNameFlag = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    if (mode > 0)
                        mode--;
                    else
                        mode = QUIT;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {

                    if (mode < QUIT)
                        mode++;
                    else
                        mode = PLAY;
                }
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    quit = true;
                }
            }
            else if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN)
            {
                int mouseX, mouseY;
                int buttons = SDL_GetMouseState(&mouseX, &mouseY);
                if (centerOfScreenX - (playW / 2) < mouseX && mouseX < centerOfScreenX + (playW / 2) && playY - (pGame->world.tileSize / 2) < mouseY && mouseY < playY + (pGame->world.tileSize / 2))
                {
                    mode = PLAY;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        quit = true;
                    }
                }

                else if (centerOfScreenX - (levelEditW / 2) < mouseX && mouseX < centerOfScreenX + (levelEditW / 2) && editY - (pGame->world.tileSize / 2) < mouseY && mouseY < editY + (pGame->world.tileSize / 2))
                {
                    mode = EDIT;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        quit = true;
                    }
                }
                else if (centerOfScreenX - (quitW / 2) < mouseX && mouseX < centerOfScreenX + (quitW / 2) && quitY - (pGame->world.tileSize / 2) < mouseY && mouseY < quitY + (pGame->world.tileSize / 2))
                {
                    mode = QUIT;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        quit = true;
                    }
                }
                else if (centerOfScreenX - (joinServerW / 2) < mouseX && mouseX < centerOfScreenX + (joinServerW / 2) && joinY - (pGame->world.tileSize / 2) < mouseY && mouseY < joinY + (pGame->world.tileSize / 2))
                {
                    mode = JOIN;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        quit = true;
                    }
                }
                else if (centerOfScreenX - (catSelectW / 2) < mouseX && mouseX < centerOfScreenX + (catSelectW / 2) && catselY - (pGame->world.tileSize / 2) < mouseY && mouseY < catselY + (pGame->world.tileSize / 2))
                {
                    mode = CATSEL;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        quit = true;
                    }
                }
                else if (centerOfScreenX - (hostW / 2) < mouseX && mouseX < centerOfScreenX + (hostW / 2) && hostY - (pGame->world.tileSize / 2) < mouseY && mouseY < hostY + (pGame->world.tileSize / 2))
                {
                    mode = HOST;
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        quit = true;
                    }
                }
                else if (0 < mouseX && mouseX < playerNameW && pGame->windowHeight - playerNameH < mouseY && mouseY < pGame->windowHeight)
                {
                    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
                    {
                        editPlayerName = true;
                    }
                }
            }
        }

        // EDIT PLAYERNAME ADDITION-------------------------------------------------------------------------------------------------------------------------
        if (updatePlayerNameFlag || (SDL_GetTicks() - previousSecond >= 1000) && editPlayerName)
        {
            char buffer[31];
            if (SDL_GetTicks() - previousSecond >= 1000) // DRAW A | every other second
            {
                previousSecond = SDL_GetTicks();
                if (drawLine)
                    drawLine = false;
                else
                    drawLine = true;
            }

            switch (drawLine)
            {
            case false:
                strcpy(buffer, pGame->pPlayer->name);
                break;
            case true:
                sprintf(buffer, "%s|", pGame->pPlayer->name);
                break;
            }

            if (!buffer[0])
            {
                TTF_SizeText(pGame->ui.pFpsFont, buffer, &playerNameW, &playerNameH);
                freeText(pName);
                pName = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, " ", playerNameW / 2, pGame->windowHeight - (playerNameH / 2));
            }
            else
            {
                TTF_SizeText(pGame->ui.pFpsFont, buffer, &playerNameW, &playerNameH);
                freeText(pName);
                pName = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, buffer, playerNameW / 2, pGame->windowHeight - (playerNameH / 2));
            }
        }
        //--------------------------------------------------------------------------------------------------------------------------------------------------

        if (mode != previousMode)
        {
            switch (previousMode)
            {
            case PLAY:
                freeText(pPlay);
                pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, playY);
                break;
            case EDIT:
                freeText(pLvlEdit);
                pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, editY);
                break;
            case QUIT:
                freeText(pQuit);
                pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, quitY);
                break;
            case JOIN:
                freeText(pJoinServer);
                pJoinServer = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, joinY);
                break;
            case CATSEL:
                freeText(pCatSelect);
                pCatSelect = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, catselY);
                break;
            case HOST:
                freeText(pHost);
                pHost = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Host Server", pGame->windowWidth / 2, hostY);
                break;
            default:
                break;
            }
        }

        deltaTime = SDL_GetTicks() - previousTime;
        if (deltaTime >= 1000 / 60)
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
            switch (mode)
            {
            case PLAY:
                freeText(pPlay);
                pPlay = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, playY);
                break;
            case EDIT:
                freeText(pLvlEdit);
                pLvlEdit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, editY);
                break;
            case QUIT:
                freeText(pQuit);
                pQuit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, quitY);
                break;
            case JOIN:
                freeText(pJoinServer);
                pJoinServer = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Join Server", pGame->windowWidth / 2, joinY);
                break;
            case CATSEL:
                freeText(pCatSelect);
                pCatSelect = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Cat Selection", pGame->windowWidth / 2, catselY);
                break;
            case HOST:
                freeText(pHost);
                pHost = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Host Server", pGame->windowWidth / 2, hostY);
                break;
            default:
                break;
            }
            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            drawText(pPlay, pGame->pRenderer);
            drawText(pLvlEdit, pGame->pRenderer);
            drawText(pQuit, pGame->pRenderer);
            drawText(pJoinServer, pGame->pRenderer);
            drawText(pCatSelect, pGame->pRenderer);
            drawText(pHost, pGame->pRenderer);
            if (pName)
                drawText(pName, pGame->pRenderer); // PLAYERNAME ADDITION TEST
            SDL_RenderPresent(pGame->pRenderer);
        }
    }
    freeText(pPlay);
    freeText(pLvlEdit);
    freeText(pQuit);
    freeText(pJoinServer);
    freeText(pCatSelect);
    freeText(pHost);
    freeText(pName); // PLAYERNAME ADDITION TEST

    return mode;
}

int serverSelectMenu(Game *pGame)
{
    // Setup UI
    unsigned int marginW = pGame->windowWidth / 32;
    unsigned int marginH = pGame->windowHeight / 32;
    unsigned int areaW = pGame->windowWidth - (2 * marginW);
    unsigned int areaH = pGame->windowHeight - (2 * marginH);
    unsigned int areaX = marginW;
    unsigned int areaY = marginH;
    unsigned int areaCenterX = areaW / 2;
    unsigned int areaCenterY = areaH / 2;
    unsigned int buttonW = areaW / 5;
    unsigned int buttonH = areaH / 5;
    unsigned int buttonX = areaX;
    unsigned int buttonY = areaY;

    SDL_Rect area = {areaX, areaY, areaW, areaH};

    SDL_Rect buttons[25];
    int nrOfButtons = 25;
    for (int i = 0; i < nrOfButtons; i++)
    {
        buttons[i].x = buttonX + (buttonW * (i % 5));
        buttons[i].y = buttonY + (buttonH * (i / 5));
        buttons[i].w = buttonW;
        buttons[i].h = buttonH;
    }

    TTF_Font *pLocalFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pGame->world.tileSize / 4);
    Text *pCheckLocal = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Press \"Scan network\" to start a scan", areaCenterX, areaCenterY);
    Text *pExitText = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Search", buttons[24].x + (buttons[24].w / 2), buttons[24].y + (buttons[24].h / 2));
    Text *pStartScanText = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Scan network", buttons[19].x + (buttons[19].w / 2), buttons[19].y + (buttons[19].h / 2));

    //
    // Setup Net search
    pthread_t scanNetThread;
    LocalServer localServerInfo;
    localServerInfo.foundServer = false;
    localServerInfo.searchDone = true;
    localServerInfo.nrOfServersFound = 0;

    bool searchResultChecked = true;
    bool startScan = false;
    //

    int mouseX, mouseY, selected = -1;

    Uint32 prevUpdateTick = 0;
    bool exit = false;
    while (!exit || !localServerInfo.searchDone) // wait for search threads
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                break;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                Uint32 mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
                if (mouseButtons & SDL_BUTTON(SDL_BUTTON_LEFT))
                {
                    bool mouseIsInsideArea = (mouseX > areaX && mouseX < areaX + areaW) && (mouseY > areaY && mouseY < areaY + areaH);
                    if (mouseIsInsideArea)
                    {
                        for (int i = 0; i < nrOfButtons; i++)
                        {
                            bool mouseIsInButton = (mouseX > buttons[i].x && mouseX < buttons[i].x + buttons[i].w) && (mouseY > buttons[i].y && mouseY < buttons[i].y + buttons[i].h);
                            if (mouseIsInButton)
                            {
                                selected = i;
                                printf("mouse is in button %d\n", i);
                                switch (selected)
                                {
                                case 0:
                                    break;
                                case 24:
                                    exit = true;
                                    break;
                                case 19:
                                    startScan = true;
                                    break;
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (startScan && localServerInfo.searchDone)
        {
            startScan = false;
            localServerInfo.foundServer = false;
            localServerInfo.searchDone = false;
            localServerInfo.nrOfServersFound = 0;
            searchResultChecked = false;

            pthread_create(&scanNetThread, NULL, scanForGamesOnLocalNetwork, &localServerInfo);
            freeText(pCheckLocal);
            pCheckLocal = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Checking Local network...", areaCenterX, areaCenterY);
        }
        Uint32 deltaTime = SDL_GetTicks() - prevUpdateTick;
        if (deltaTime >= 1000 / 60)
        {
            prevUpdateTick = SDL_GetTicks();

            if (localServerInfo.searchDone && !searchResultChecked)
            {
                freeText(pCheckLocal);
                char buffer[100];
                searchResultChecked = true;
                pthread_join(scanNetThread, NULL);
                if (localServerInfo.foundServer)
                {

                    for (int i = 0; i < localServerInfo.nrOfServersFound; i++)
                    {
                        printf("IP: %s\n", localServerInfo.ppIpStringList[i]);
                        free(localServerInfo.ppIpStringList[i]);
                    }
                    free(localServerInfo.ppIpStringList);

                    sprintf(buffer, "Found %d servers!", localServerInfo.nrOfServersFound);
                    pCheckLocal = createText(pGame->pRenderer, 0, 255, 0, pLocalFont, buffer, areaCenterX, areaCenterY);
                }
                else
                {
                    strcpy(buffer, "No servers found :(");
                    pCheckLocal = createText(pGame->pRenderer, 255, 0, 0, pLocalFont, buffer, areaCenterX, areaCenterY);
                }
            }

            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            SDL_SetRenderDrawColor(pGame->pRenderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(pGame->pRenderer, &area);
            for (int i = 0; i < nrOfButtons; i++)
            {
                if (selected != i)
                    SDL_RenderDrawRect(pGame->pRenderer, &buttons[i]);
                else
                    SDL_RenderFillRect(pGame->pRenderer, &buttons[i]);
            }

            drawText(pCheckLocal, pGame->pRenderer);
            drawText(pExitText, pGame->pRenderer);
            if (localServerInfo.searchDone)
                drawText(pStartScanText, pGame->pRenderer);

            SDL_RenderPresent(pGame->pRenderer);
        }
    }
    printf("Exiting server select menu...\n");
    freeText(pStartScanText);
    freeText(pExitText);
    freeText(pCheckLocal);
    TTF_CloseFont(pLocalFont);

    return 0;
}