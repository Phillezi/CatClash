

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


/*else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
{

    if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
    {
        exit = true;
        break;
    }
}*/

/*
else if ((mouseY >= saveButton.y && mouseX >= saveButton.x) && (mouseY <= (saveButton.y + saveButton.h) && mouseX <= (saveButton.x + saveButton.w)))
{
    printf("What would you like to name the file?\n: ");
    scanf(" %30s", fileName);
    saveToFile(map, fileName);
    printf("saved file\n");
}
else if ((mouseY >= openButton.y && mouseX >= openButton.x) && (mouseY <= (openButton.y + openButton.h) && mouseX <= (openButton.x + openButton.w)))
{
    printf("You pressed the OPEN button!\n");
}
*/
/*
          else
          {
              // SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[19], NULL, &pGame->map[i].wall);
          }
  */

/*
if (game.serverIsHosted)
{
   printf("Closing server...\n");
   pthread_cancel(game.serverThread);
   pthread_join(game.serverThread, NULL);
   game.serverIsHosted = false;
}
*/

/*
if (game.serverIsHosted == false)
{
    if (testSelectMenu(&game, mapName))
        break;
    if (!pthread_create(&game.serverThread, NULL, MThostServer, (void *)mapName))
    {
        game.serverIsHosted = true;
    }
}
else
{
    printf("SERVER IS ALREADY HOSTED\n");
}
*/

/*
if (pGame->config.multiThreading){
    pthread_create(&renderThread, NULL, updateScreen, (void *)pGame);
}
*/

// if (pGame->config.multiThreading)
//  pthread_join(renderThread, NULL);
// else

/*
else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
{

    if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
    {
        exit = true;
        break;
    }
    else
    {
        if (pGame->serverIsHosted)
        {
            printf("Closing server...\n");
            pthread_cancel(pGame->serverThread);
            pthread_join(pGame->serverThread, NULL);
            pGame->serverIsHosted = false;
        }
    }
}*/

/* if (pGame->pTileTextures[1])
     SDL_DestroyTexture(pGame->pTileTextures[1]);
 if (pGame->pTileTextures[2])
     SDL_DestroyTexture(pGame->pTileTextures[2]);
 if (pGame->pTileTextures[3])
     SDL_DestroyTexture(pGame->pTileTextures[3]);*/

// SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type)], 150, 150, 150);

// SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type)], 255, 255, 255);

/*else
{
    temp = pGame->map[i].wall;
    //if(i > MAPSIZE - 1 && pGame->map[i - MAPSIZE].type > 0)
    //    temp.h = (float)pGame->map[i].wall.h - ((float)pGame->world.tileSize * pGame->world.angle);
    temp.h += pGame->map[i].wall.h - ((float)pGame->world.tileSize * pGame->world.angle);
    SDL_RenderFillRect(pGame->pRenderer, &temp);
}*/

/*else
{
    if (getStringFromUser(text, event))
    {
        if (initMap(pGame->map, text, pGame->world.tileSize))
        {
            printf("No file found\n");
        }
        else
        {
            getPlayerSpawnPos(pGame);
            exit = true;
        }
    }
    else
    {
        if (text[0])
        {
            freeText(pMap);
            pMap = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, text, pGame->windowWidth / 2, pGame->windowHeight / 2);
        }
    }
}*/

/*else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
{

    if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
    {
        exit = true;
        break;
    }
    else
    {
        if (pGame->serverIsHosted)
        {
            printf("Closing server...\n");
            pthread_cancel(pGame->serverThread);
            pthread_join(pGame->serverThread, NULL);
            pGame->serverIsHosted = false;
        }
    }
}*/

/*else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
{
    if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
    {
        exit = true;
        TTF_CloseFont(listFont);
        for (int i = 0; i < len; i++)
            freeText(pText[i]);
        freeTextList(strArr, len);
        return 1;
        break;
    }
    else
    {
        if (pGame->serverIsHosted)
        {
            printf("Closing server...\n");
            pthread_cancel(pGame->serverThread);
            pthread_join(pGame->serverThread, NULL);
            pGame->serverIsHosted = false;
        }
    }
}*/

/*else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
{
    if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
    {
        exit = true;
    }
    else
    {
        if (pGame->serverIsHosted)
        {
            printf("Closing server...\n");
            pthread_cancel(pGame->serverThread);
            pthread_join(pGame->serverThread, NULL);
            pGame->serverIsHosted = false;
        }
    }
}*/

/*else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
{

    if (SDL_GetWindowID(pGame->pWindow) == event.window.windowID)
    {
        mode = QUIT;
        quit = true;
    }
    else
    {
        if (pGame->serverIsHosted)
        {
            printf("Closing server...\n");
            pthread_cancel(pGame->serverThread);
            pthread_join(pGame->serverThread, NULL);
            pGame->serverIsHosted = false;
        }
    }
}*/

/*
if (!checkCollision(*pPlayer, map, *pPrevKeypressed))
    {
        movePlayer(pPlayer, *pPrevKeypressed);
    }
    else
    {
        printf("COLLISION %c\n", *pPrevKeypressed);
    }
    */

/*
pGame->player.x = spawnTile.x;
pGame->player.y = spawnTile.y;
pGame->pPlayer->rect.x = spawnTile.x; // windowWidth / 2;
pGame->pPlayer->rect.y = spawnTile.y; // windowHeight / 2;
*/

/*
 pNew_arr[size].id = data.id;
 pNew_arr[size].x = data.x;
 pNew_arr[size].y = data.y;
 pNew_arr[size].idle = data.idle;
 pNew_arr[size].prevKeyPressed = data.direction;
 pNew_arr[size].charge = data.charge;
 pNew_arr[size].charging = data.charging;
 strcpy(pNew_arr[size].name, "Allocated");
 */