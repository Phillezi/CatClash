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