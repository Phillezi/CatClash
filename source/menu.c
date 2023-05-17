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
    Uint8 nrOfButtons = 25;
    // Setup UI
    unsigned int marginW = pGame->windowWidth / 32;
    unsigned int marginH = pGame->windowHeight / 32;
    unsigned int areaW = pGame->windowWidth - (2 * marginW);
    unsigned int areaH = pGame->windowHeight - (2 * marginH);
    unsigned int areaX = marginW;
    unsigned int areaY = marginH;
    unsigned int areaCenterX = areaX + areaW / 2;
    unsigned int areaCenterY = areaY + areaH / 2;
    unsigned int buttonMarginW = marginW / (int)sqrt(nrOfButtons);
    unsigned int buttonMarginH = marginH / (int)sqrt(nrOfButtons);
    unsigned int buttonW = areaW / (int)sqrt(nrOfButtons) - (2 * buttonMarginW);
    unsigned int buttonH = areaH / (int)sqrt(nrOfButtons) - (2 * buttonMarginH);
    unsigned int buttonX = areaX + (buttonMarginW * ((int)sqrt(nrOfButtons) + 1)) / 2;
    unsigned int buttonY = areaY + (buttonMarginH * ((int)sqrt(nrOfButtons) + 1)) / 2;

    unsigned int firstInButtonRow = (unsigned int)sqrt(nrOfButtons);
    unsigned int lastInButtonRow = ((unsigned int)sqrt(nrOfButtons) - 1);
    unsigned int lastButton = nrOfButtons - 1;

    SDL_Rect area = {areaX, areaY, areaW, areaH};

    SDL_Rect buttons[nrOfButtons];
    for (int i = 0; i < nrOfButtons; i++)
    {
        buttons[i].x = buttonX + (buttonW * (i % firstInButtonRow)) + (buttonMarginW * (i % firstInButtonRow));
        buttons[i].y = buttonY + (buttonH * (i / firstInButtonRow)) + (buttonMarginH * (i / firstInButtonRow));
        buttons[i].w = buttonW;
        buttons[i].h = buttonH;
    }

    unsigned int fontSize = pGame->world.tileSize / 4;
    TTF_Font *pLocalFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pGame->world.tileSize / 4);
    Text *pCheckLocal = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Press \"Scan network\" to start a scan", areaCenterX, buttons[2].y + buttons[2].h / 2);
    Text *pExitText = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Back", buttons[firstInButtonRow * 4].x + (buttons[firstInButtonRow * 4].w / 2), buttons[firstInButtonRow * 4].y + (buttons[firstInButtonRow * 4].h / 2));
    Text *pSearchText = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Search", buttons[lastButton].x + (buttons[lastButton].w / 2), buttons[lastButton].y + (buttons[lastButton].h / 2));
    Text *pStartScanText = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Scan network", buttons[lastButton - 1].x + (buttons[lastButton - 1].w / 2), buttons[lastButton - 1].y + (buttons[lastButton - 1].h / 2));
    Text *pStartListScanText = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Scan saved", buttons[lastButton - 2].x + (buttons[lastButton - 2].w / 2), buttons[lastButton - 2].y + (buttons[lastButton - 2].h / 2));

    Text *pIpText[firstInButtonRow];
    Text *pAmountOfPlayersText[firstInButtonRow];

    Text *pIpText2[firstInButtonRow];
    Text *pAmountOfPlayersText2[firstInButtonRow];

    //
    // Setup Net search
    pthread_t scanNetThread, scanListThread;
    LocalServer localServerInfo;
    localServerInfo.foundServer = false;
    localServerInfo.searchDone = true;
    localServerInfo.nrOfServersFound = 0;
    localServerInfo.ppIpStringList = NULL;
    localServerInfo.pPlayersOnline = NULL;

    LocalServer savedServerInfo;
    savedServerInfo.foundServer = false;
    savedServerInfo.searchDone = true;
    savedServerInfo.nrOfServersFound = 0;
    savedServerInfo.ppIpStringList = NULL;
    savedServerInfo.pPlayersOnline = NULL;

    bool searchResultChecked = true;
    bool searchResultChecked2 = true;
    bool startScan = false;
    bool startScan2 = false;
    //

    int mouseX, mouseY, selected = -1;
    int returnValue = 0;

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
                                if (selected == lastButton - 1)
                                {
                                    startScan = true;
                                    break;
                                }
                                else if (selected == lastButton - 2)
                                {
                                    startScan2 = true;
                                    break;
                                }
                                else if (selected == firstInButtonRow * 4)
                                {
                                    exit = true;
                                    returnValue = 0;
                                    break;
                                }
                                else if (selected == lastButton)
                                {
                                    exit = true;
                                    returnValue = 1;
                                    break;
                                }
                                else if (selected >= firstInButtonRow && selected < firstInButtonRow * 2)
                                {
                                    if (localServerInfo.nrOfServersFound > selected - firstInButtonRow)
                                    {
                                        pGame->pClient = createClient(localServerInfo.ppIpStringList[selected - firstInButtonRow], 1234, 0, 100, 100);
                                        if (connectToServer(pGame))
                                        {
                                            printf("Error: Could not join server\n");
                                            exit = true;
                                            free(pGame->pClient);
                                            returnValue = 0;
                                        }
                                        printf("Connected to Server\n");
                                        initMapFromTCP(pGame->map, pGame->world.tileSize);
                                        getPlayerSpawnPos(pGame);
                                        SDLNet_ResolveHost(&pGame->serverAddress, localServerInfo.ppIpStringList[selected - firstInButtonRow], 1234);
                                        printf("Resolved UDP host\n");
                                        exit = true;
                                        returnValue = 2;
                                    }
                                    
                                }
                                else if (selected >= firstInButtonRow*2 && selected < firstInButtonRow * 3)
                                {
                                    if (savedServerInfo.nrOfServersFound > selected - firstInButtonRow * 2)
                                    {
                                        pGame->pClient = createClient(savedServerInfo.ppIpStringList[selected - firstInButtonRow * 2], 1234, 0, 100, 100);
                                        if (connectToServer(pGame))
                                        {
                                            printf("Error: Could not join server\n");
                                            exit = true;
                                            free(pGame->pClient);
                                            returnValue = 0;
                                        }
                                        printf("Connected to Server\n");
                                        initMapFromTCP(pGame->map, pGame->world.tileSize);
                                        getPlayerSpawnPos(pGame);
                                        SDLNet_ResolveHost(&pGame->serverAddress, savedServerInfo.ppIpStringList[selected - firstInButtonRow * 2], 1234);
                                        printf("Resolved UDP host\n");
                                        exit = true;
                                        returnValue = 2;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (startScan && localServerInfo.searchDone && savedServerInfo.searchDone)
        {
            for (int i = 0; i < localServerInfo.nrOfServersFound; i++)
            {
                if (i < firstInButtonRow)
                {
                    freeText(pIpText[i]);
                    freeText(pAmountOfPlayersText[i]);
                }

                free(localServerInfo.ppIpStringList[i]);
            }
            free(localServerInfo.ppIpStringList);
            localServerInfo.ppIpStringList = NULL;
            free(localServerInfo.pPlayersOnline);
            localServerInfo.pPlayersOnline = NULL;

            startScan = false;
            localServerInfo.foundServer = false;
            localServerInfo.searchDone = false;
            localServerInfo.nrOfServersFound = 0;
            searchResultChecked = false;

            pthread_create(&scanNetThread, NULL, scanForGamesOnLocalNetwork, &localServerInfo);
            freeText(pCheckLocal);
            pCheckLocal = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Checking Local network...", areaCenterX, buttons[2].y + buttons[2].h / 2);
        }
        if (startScan2 && localServerInfo.searchDone && savedServerInfo.searchDone)
        {
            for (int i = 0; i < savedServerInfo.nrOfServersFound; i++)
            {
                if (i < firstInButtonRow)
                {
                    freeText(pIpText2[i]);
                    freeText(pAmountOfPlayersText2[i]);
                }

                free(savedServerInfo.ppIpStringList[i]);
            }
            free(savedServerInfo.ppIpStringList);
            savedServerInfo.ppIpStringList = NULL;
            free(savedServerInfo.pPlayersOnline);
            savedServerInfo.pPlayersOnline = NULL;

            startScan2 = false;
            savedServerInfo.foundServer = false;
            savedServerInfo.searchDone = false;
            savedServerInfo.nrOfServersFound = 0;
            searchResultChecked2 = false;

            pthread_create(&scanListThread, NULL, scanForGamesFromSavedList, &savedServerInfo);
            freeText(pCheckLocal);
            pCheckLocal = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, "Checking Local network...", areaCenterX, buttons[2].y + buttons[2].h / 2);
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
                        if (i < firstInButtonRow)
                        {
                            pIpText[i] = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, localServerInfo.ppIpStringList[i], buttons[5 + i].x + (buttons[5 + i].w / 2), buttons[5 + i].y + (fontSize / 2) + (fontSize / 4));
                            sprintf(buffer, "%d players", localServerInfo.pPlayersOnline[i]);
                            pAmountOfPlayersText[i] = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, buffer, buttons[5 + i].x + (buttons[5 + i].w / 2), buttons[5 + i].y + (buttons[5 + i].h / 2));
                        }
                    }

                    sprintf(buffer, "Found %d servers!", localServerInfo.nrOfServersFound);
                    pCheckLocal = createText(pGame->pRenderer, 0, 255, 0, pLocalFont, buffer, areaCenterX, buttons[2].y + buttons[2].h / 2);
                }
                else
                {
                    strcpy(buffer, "No servers found :(");
                    pCheckLocal = createText(pGame->pRenderer, 255, 0, 0, pLocalFont, buffer, areaCenterX, buttons[2].y + buttons[2].h / 2);
                }
            }

            if (savedServerInfo.searchDone && !searchResultChecked2)
            {
                freeText(pCheckLocal);
                char buffer[100];
                searchResultChecked2 = true;
                pthread_join(scanListThread, NULL);
                if (savedServerInfo.foundServer)
                {

                    for (int i = 0; i < savedServerInfo.nrOfServersFound; i++)
                    {
                        printf("IP: %s\n", savedServerInfo.ppIpStringList[i]);
                        if (i < firstInButtonRow)
                        {
                            pIpText2[i] = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, savedServerInfo.ppIpStringList[i], buttons[5 * 2 + i].x + (buttons[5 * 2 + i].w / 2), buttons[5 * 2 + i].y + (fontSize / 2) + (fontSize / 4));
                            sprintf(buffer, "%d players", savedServerInfo.pPlayersOnline[i]);
                            pAmountOfPlayersText2[i] = createText(pGame->pRenderer, 0, 0, 0, pLocalFont, buffer, buttons[5 * 2 + i].x + (buttons[5 * 2 + i].w / 2), buttons[5 * 2 + i].y + (buttons[5 * 2 + i].h / 2));
                        }
                    }

                    sprintf(buffer, "Found %d servers!", savedServerInfo.nrOfServersFound);
                    pCheckLocal = createText(pGame->pRenderer, 0, 255, 0, pLocalFont, buffer, areaCenterX, buttons[2].y + buttons[2].h / 2);
                }
                else
                {
                    strcpy(buffer, "No servers found :(");
                    pCheckLocal = createText(pGame->pRenderer, 255, 0, 0, pLocalFont, buffer, areaCenterX, buttons[2].y + buttons[2].h / 2);
                }
            }

            SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pGame->pRenderer);
            SDL_SetRenderDrawColor(pGame->pRenderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(pGame->pRenderer, &area);
            for (int i = firstInButtonRow; i < nrOfButtons; i++)
            {
                if (selected != i)
                    SDL_RenderDrawRect(pGame->pRenderer, &buttons[i]);
                else
                    SDL_RenderFillRect(pGame->pRenderer, &buttons[i]);
            }
            if (localServerInfo.searchDone)
            {
                for (int i = 0; i < localServerInfo.nrOfServersFound; i++)
                {
                    if (i < firstInButtonRow)
                    {
                        drawText(pIpText[i], pGame->pRenderer);
                        drawText(pAmountOfPlayersText[i], pGame->pRenderer);
                    }
                }
            }
            if (savedServerInfo.searchDone)
            {
                for (int i = 0; i < savedServerInfo.nrOfServersFound; i++)
                {
                    if (i < firstInButtonRow)
                    {
                        drawText(pIpText2[i], pGame->pRenderer);
                        drawText(pAmountOfPlayersText2[i], pGame->pRenderer);
                    }
                }
            }

            drawText(pCheckLocal, pGame->pRenderer);
            drawText(pExitText, pGame->pRenderer);
            drawText(pSearchText, pGame->pRenderer);
            if (localServerInfo.searchDone)
                drawText(pStartScanText, pGame->pRenderer);
            if (savedServerInfo.searchDone)
                drawText(pStartListScanText, pGame->pRenderer);

            SDL_RenderPresent(pGame->pRenderer);
        }
    }
    printf("Exiting server select menu...\n");
    for (int i = 0; i < localServerInfo.nrOfServersFound; i++)
    {
        if (i < firstInButtonRow)
        {
            freeText(pIpText[i]);
            freeText(pAmountOfPlayersText[i]);
        }

        free(localServerInfo.ppIpStringList[i]);
    }
    free(localServerInfo.ppIpStringList);
    localServerInfo.ppIpStringList = NULL;

    free(localServerInfo.pPlayersOnline);
    localServerInfo.pPlayersOnline = NULL;

    for (int i = 0; i < savedServerInfo.nrOfServersFound; i++)
    {
        if (i < firstInButtonRow)
        {
            freeText(pIpText2[i]);
            freeText(pAmountOfPlayersText2[i]);
        }

        free(savedServerInfo.ppIpStringList[i]);
    }
    free(savedServerInfo.ppIpStringList);
    savedServerInfo.ppIpStringList = NULL;

    free(savedServerInfo.pPlayersOnline);
    savedServerInfo.pPlayersOnline = NULL;

    freeText(pStartScanText);
    freeText(pStartListScanText);
    freeText(pExitText);
    freeText(pSearchText);
    freeText(pCheckLocal);
    TTF_CloseFont(pLocalFont);

    return returnValue;
}

int serverLobby(Game *pGame)
{
    bool playerWasConnected = false, exit = false;
    Uint32 prevUDPTransfer = 0, prevUpdateTick = 0, deltaTime = 0;
    int returnValue = 0, nrOfPlayersDisplayed = -1;

    SDL_Rect *pIcons = (SDL_Rect *)malloc(sizeof(SDL_Rect));
    pIcons[0].x = (pGame->windowWidth - pGame->world.tileSize) / 2;
    pIcons[0].y = pGame->windowHeight / 2;
    pIcons[0].w = pGame->world.tileSize;
    pIcons[0].h = pGame->world.tileSize;

    Text *pTitleText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Server Lobby", pGame->windowWidth / 2, (3 * pGame->world.tileSize) / 2);
    Text *pNrOfPlayersText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Loading...", pGame->windowWidth / 2, 2 * (3 * pGame->world.tileSize) / 2);
    Text *pInfoText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Press space", pGame->windowWidth / 2, pGame->windowHeight - (3 * pGame->world.tileSize) / 2);

    pGame->pPlayer->state = ALIVE;
    pGame->pPlayer->hp = 255;

    if (pGame->isConnected && !pGame->packetAllocatedFlag)
    {
        pGame->pPacket = SDLNet_AllocPacket(sizeof(PlayerUdpPkg));
        if (pGame->socketDesc = SDLNet_UDP_Open(0))
        {
            printf("UDP init\n");
        }
        pGame->packetAllocatedFlag = true;
    }
    while (!exit)
    {
        deltaTime = SDL_GetTicks() - prevUpdateTick;
        if (deltaTime >= 1000 / 60)
        {
            prevUpdateTick = SDL_GetTicks();
            if (pGame->isConnected)
            {
                checkTCP(pGame);
                playerWasConnected = true;
            }
            else if (playerWasConnected)
            {
                playerWasConnected = false;
                pGame->nrOfPlayers = 0;
                if (pGame->pMultiPlayer)
                    free(pGame->pMultiPlayer);
                SDLNet_TCP_Close(pGame->pClient->socketTCP);
                SDLNet_FreeSocketSet(pGame->pClient->sockets);
                free(pGame->pClient);
            }

            int keepAliveDelta = SDL_GetTicks() - prevUDPTransfer;
            if (pGame->isConnected)
            {
                if (keepAliveDelta > 4500)
                {
                    prevUDPTransfer = SDL_GetTicks();
                    sendData(pGame);
                }
            }
            if (pGame->isConnected)
                getPlayerData(pGame);

            if (pGame->nrOfPlayers != nrOfPlayersDisplayed)
            {

                char buffer[31];
                freeText(pNrOfPlayersText);
                switch (pGame->nrOfPlayers)
                {
                case 0:
                    strcpy(buffer, "Just you");
                    break;
                case 1:
                    if (strlen(pGame->pMultiPlayer[0].name) + strlen("You and ") < 30 && pGame->pMultiPlayer[0].name[0])
                    {
                        sprintf(buffer, "You and %s", pGame->pMultiPlayer[0].name);
                        break;
                    }
                    else
                        strcpy(buffer, "You and another player");
                    break;
                default:
                    sprintf(buffer, "You and %d Players", pGame->nrOfPlayers);
                    break;
                }

                pIcons = (SDL_Rect *)realloc(pIcons, (pGame->nrOfPlayers + 1) * sizeof(SDL_Rect));

                int x = (pGame->windowWidth - ((pGame->nrOfPlayers + 1) * pGame->world.tileSize)) / 2;
                int y = pGame->windowHeight / 2;

                for (int i = 0; i < pGame->nrOfPlayers + 1; i++)
                {
                    pIcons[i].x = x + (i * pGame->world.tileSize);
                    pIcons[i].y = y;
                    pIcons[i].w = pGame->world.tileSize;
                    pIcons[i].h = pGame->world.tileSize;
                }

                nrOfPlayersDisplayed = pGame->nrOfPlayers;
                pNrOfPlayersText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, buffer, pGame->windowWidth / 2, 2 * (3 * pGame->world.tileSize) / 2);
            }

            SDL_SetRenderDrawColor(pGame->pRenderer, 200, 200, 200, 255);
            SDL_RenderClear(pGame->pRenderer);

            drawText(pTitleText, pGame->pRenderer);
            drawText(pNrOfPlayersText, pGame->pRenderer);
            drawText(pInfoText, pGame->pRenderer);

            for (int i = 0; i < pGame->nrOfPlayers + 1; i++)
            {

                if (i < pGame->nrOfPlayers)
                {
                    SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
                    SDL_RenderDrawRect(pGame->pRenderer, &pIcons[i]);
                    SDL_RenderCopy(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[pGame->pMultiPlayer[i].id][0], &pIcons[i]);
                }
                else
                {
                    SDL_SetRenderDrawColor(pGame->pRenderer, 255, 0, 0, 255);
                    SDL_RenderDrawRect(pGame->pRenderer, &pIcons[i]);
                    SDL_RenderCopy(pGame->pRenderer, pGame->pPlayerTexture, &pGame->gSpriteClips[pGame->pPlayer->id][0], &pIcons[i]);
                }
            }

            SDL_RenderPresent(pGame->pRenderer);
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                returnValue = 1;
                break;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_SPACE)
                {
                    exit = true;
                    returnValue = 0;
                }
            }
        }
    }
    if (pGame->isConnected && returnValue)
    {
        SDLNet_TCP_Close(pGame->pClient->socketTCP);
        SDLNet_FreeSocketSet(pGame->pClient->sockets);
        free(pGame->pClient);
    }
    freeText(pTitleText);
    freeText(pNrOfPlayersText);
    freeText(pInfoText);
    free(pIcons);
    pGame->pPlayer->state = ALIVE;
    pGame->pPlayer->hp = 255;
    return returnValue;
}
