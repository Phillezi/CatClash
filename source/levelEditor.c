#include "levelEditor.h"
#include "text.h"

int levelEditor(Game *pGame)
{
    int previousTime = 0;
    int mouseX, mouseY;

    bool exit = false;
    while (!exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                break;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                lvlhandleZoom(pGame, event.wheel.y);
            }
        }
        int timeDelta = SDL_GetTicks() - previousTime;
        if (timeDelta >= 1000 / FPS)
        {
            previousTime = SDL_GetTicks();
            lvlhandleInput(pGame, &mouseX, &mouseY);

            lvlupdateScreen(pGame, mouseX, mouseY);
        }
    }
    // Reset zoom
    while (pGame->map[0].wall.w > pGame->world.tileSize)
    {
        lvlhandleZoom(pGame, -1);
    }
    while (pGame->map[0].wall.w < pGame->world.tileSize)
    {
        lvlhandleZoom(pGame, 1);
    }
    return 0;
}
void lvlhandleZoom(Game *pGame, int mouseWheelY)
{
    if (mouseWheelY < 0) // scroll up
    {
        if (pGame->map[0].wall.w >= 5)
        {
            int zoomAmount = (2 * ((float)pGame->map[0].wall.w / pGame->world.tileSize)) + 1;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                pGame->map[i].wall.w -= zoomAmount;
                pGame->map[i].wall.h -= zoomAmount;
                pGame->map[i].wall.x -= ((i % (MAPSIZE)) * zoomAmount);
                pGame->map[i].wall.y -= ((i / (MAPSIZE)) * zoomAmount);
            }
        }
    }
    else if (mouseWheelY > 0) // scroll down
    {
        if (pGame->map[0].wall.w < pGame->windowWidth)
        {
            int zoomAmount = (2 * ((float)pGame->map[0].wall.w / pGame->world.tileSize)) + 1;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                pGame->map[i].wall.w += zoomAmount;
                pGame->map[i].wall.h += zoomAmount;
                pGame->map[i].wall.x += ((i % (MAPSIZE)) * zoomAmount);
                pGame->map[i].wall.y += ((i / (MAPSIZE)) * zoomAmount);
            }
        }
    }
}
void lvlhandleInput(Game *pGame, int *pMouseX, int *pMouseY)
{
    int buttons = SDL_GetMouseState(pMouseX, pMouseY);

    // int movementAmount = (2 * ((float)pGame->map[0].wall.w / pGame->world.tileSize)) + 1;

    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    if (currentKeyStates[SDL_SCANCODE_0])
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 0;
    }
    if (currentKeyStates[SDL_SCANCODE_1])
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 1;
    }
    else if (currentKeyStates[SDL_SCANCODE_2])
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 2;
    }
    else if (currentKeyStates[SDL_SCANCODE_3])
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 3;
    }
    else if (currentKeyStates[SDL_SCANCODE_4])
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 4;
    }
    else if (currentKeyStates[SDL_SCANCODE_S] && currentKeyStates[SDL_SCANCODE_LCTRL])
    {
        char fileName[31];
        printf("What would you like to name the file?\n: ");
        scanf(" %30s", fileName);
        saveToFile(pGame->map, fileName);
        printf("saved file\n");
    }
    else if (currentKeyStates[SDL_SCANCODE_DELETE])
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].type = 0;
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_INSERT])
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].type = 1;
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_W])
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y += pGame->movementAmount;
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_S])
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y -= pGame->movementAmount;
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_D])
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x -= pGame->movementAmount;
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_A])
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x += pGame->movementAmount;
        }
    }
    else if (currentKeyStates[SDL_SCANCODE_M])
    {
        for (int i = 0; i < MAPSIZE / 2; i++)
        {
            for (int row = 0; row < MAPSIZE; row++)
            {
                pGame->map[(MAPSIZE * row) + ((MAPSIZE / 2) + i)].type = pGame->map[(MAPSIZE * row) + ((MAPSIZE / 2) - (i + 1))].type;
            }
        }
    }

    // float scale = ((float)map[0].wall.w / TILESIZE);

    if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 1;
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
    }

    if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
    {
        if ((*pMouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE) && (*pMouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE))
            pGame->map[(((*pMouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((*pMouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].type = 0;
    }
}
void lvlupdateScreen(Game *pGame, int mouseX, int mouseY)
{
    SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pGame->pRenderer);
    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 0, 255);

    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (((pGame->map[i].wall.x <= pGame->windowWidth) && (pGame->map[i].wall.x + pGame->map[i].wall.w >= 0)) && ((pGame->map[i].wall.y <= pGame->windowHeight) && (pGame->map[i].wall.y + pGame->map[i].wall.w >= 0)))
        {
            if (pGame->map[i].type > 0)
                SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[pGame->map[i].type - 1], NULL, &pGame->map[i].wall);
            else if (pGame->map[i].type == -1)
            {
                SDL_SetRenderDrawColor(pGame->pRenderer, 0, 155, 0, 255);
                SDL_RenderFillRect(pGame->pRenderer, &pGame->map[i].wall);
            }
        }
    }
    // Draw grid
    SDL_SetRenderDrawColor(pGame->pRenderer, 100, 100, 100, 255);
    for (int line = 0; line < pGame->map[0].wall.w * MAPSIZE; line += pGame->map[0].wall.w)
    {
        SDL_RenderDrawLine(pGame->pRenderer, pGame->map[0].wall.x, (line + pGame->map[0].wall.y), pGame->map[MAPSIZE - 1].wall.x + pGame->map[0].wall.w, (line + pGame->map[0].wall.y));
        SDL_RenderDrawLine(pGame->pRenderer, (line + pGame->map[0].wall.x), pGame->map[0].wall.y, (line + pGame->map[0].wall.x), pGame->map[MAPSIZE * MAPSIZE - 1].wall.y + pGame->map[0].wall.w);
    }

    if (((mouseY - pGame->map[0].wall.y) < (pGame->map[0].wall.w * MAPSIZE)) && ((mouseX - pGame->map[0].wall.x) < (pGame->map[0].wall.w * MAPSIZE)))
    {
        SDL_SetRenderDrawColor(pGame->pRenderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(pGame->pRenderer, &pGame->map[(((mouseY - pGame->map[0].wall.y) / pGame->map[0].wall.w * MAPSIZE) + ((mouseX - pGame->map[0].wall.x) / pGame->map[0].wall.w))].wall);
    }

    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 255, 0, 255);
    // SDL_RenderFillRect(pRenderer, &saveButton);

    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);
    // SDL_RenderFillRect(pRenderer, &openButton);

    SDL_RenderPresent(pGame->pRenderer);
}
void saveToFile(Tile map[], char fileName[])
{
    char location[100];
    if (strstr(fileName, ".txt"))
        sprintf(location, "%s%s", SAVE_MAP_PATH, fileName);
    else
        sprintf(location, "%s%s.txt", SAVE_MAP_PATH, fileName);
    printf("Saved at: %s\n", location);
    FILE *fp;
    fp = fopen(location, "w+");
    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                fprintf(fp, "%d ", map[row * MAPSIZE + col].type);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR opening FILE");
    }
}