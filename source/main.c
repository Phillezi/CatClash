#include "definitions.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "init.h"
#include "text.h"
#include "levelEditor.h"

int init(Game *pGame);
int menu(Game *pGame);
int mapSelection(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);

void updateScreen(Game *pGame);
void centerPlayer(Game *pGame);
int handleInput(Game *pGame);
void movePlayer(Player *pPlayer, char direction);
int checkCollision(Player player, Tile map[], char direction, int tileSize);
SDL_Rect findEmptyTile(Tile map[]);
void getPlayerSpawnPos(Game *pGame);
int getStringFromUser(char text[], SDL_Event event);

int main(int argv, char **args)
{
    Game game;
    if (init(&game))
    {
        close(&game);
        return 1;
    }
    while (1)
    {
        switch (menu(&game))
        {
        case 0:
            if (mapSelection(&game))
                break;
            run(&game);
            break;
        case 1:
            if (mapSelection(&game))
                break;
            levelEditor(&game);
            break;
        case 2:
            close(&game);
            return 0;
            break;
        default:
            break;
        }
    }
}

int init(Game *pGame)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    pGame->config.vSync = true; // Hårdkodad

    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
    {
        printf("SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    pGame->windowWidth = (float)displayMode.w * 0.7; // 70% of avaliable space
    pGame->windowHeight = (float)displayMode.h * 0.7;

    pGame->world.tileSize = (pGame->windowHeight / MAPSIZE) * 4;

    pGame->pWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pGame->windowWidth, pGame->windowHeight, 0);
    if (!pGame->pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    if (pGame->config.vSync)
    {
        pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        printf("Vsync is enabled\n");
    }
    else
    {
        pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED);
        printf("Vsync is not enabled\n");
    }

    if (!pGame->pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    char tileTextures[TILES][20] = {"resources/Tile1.png", "resources/Tile2.png", "resources/Tile3.png", "resources/Tile4.png"};
    if (initTextureTiles(pGame->pRenderer, pGame->pWindow, pGame->pTileTextures, tileTextures, TILES) == -1)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    if (initTexturePlayer(pGame->pRenderer, pGame->pWindow, &pGame->pPlayerTexture) == -1)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    pGame->ui.pGameFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", 100);
    if (!pGame->ui.pGameFont)
    {
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }

    pGame->ui.pFpsFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pGame->world.tileSize);
    if (!pGame->ui.pFpsFont)
    {
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }

    pGame->ui.pMenuText = createText(pGame->pRenderer, 97, 181, 97, pGame->ui.pGameFont, "Press Space to play!", pGame->windowWidth / 2, pGame->windowHeight / 2);
    pGame->ui.pOverText = createText(pGame->pRenderer, 20, 197, 204, pGame->ui.pGameFont, "You Died!", pGame->windowWidth / 2, pGame->windowHeight / 2);
    if (!pGame->ui.pMenuText || !pGame->ui.pOverText)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_QueryTexture(pGame->pPlayerTexture, NULL, NULL, &pGame->player.rect.w, &pGame->player.rect.h) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    getPlayerSpawnPos(pGame);

    pGame->player.rect.w = pGame->world.tileSize;
    pGame->player.rect.h = pGame->world.tileSize;

    pGame->player.hp = 255;

    pGame->ui.chargebar.x = ((pGame->windowWidth / 2) - (MAX_CHARGE / 2));
    pGame->ui.chargebar.y = ((3 * pGame->windowHeight) / 4);
    pGame->ui.chargebar.w = pGame->player.charge;
    pGame->ui.chargebar.h = pGame->world.tileSize;

    pGame->ui.healthbar.x = ((pGame->windowWidth / 2) + (MAX_CHARGE / 2) + 5);
    pGame->ui.healthbar.y = ((3 * pGame->windowHeight) / 4);
    pGame->ui.healthbar.w = pGame->player.hp;
    pGame->ui.healthbar.h = pGame->world.tileSize;

    pGame->movementAmount = (float)pGame->world.tileSize / TILESIZE;

    return 0;
}
int menu(Game *pGame)
{
    int selectedMode = 0, previousTime = 0;
    int r = 0, g = 0, b = 0;
    Text *pPlay = malloc(sizeof(Text)), *pLvlEdit = malloc(sizeof(Text)), *pQuit = malloc(sizeof(Text));
    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 2;
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_UP)
                {
                    if (selectedMode > 0)
                        selectedMode--;
                    else
                        selectedMode = 2;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {

                    if (selectedMode < 2)
                        selectedMode++;
                    else
                        selectedMode = 0;
                }
                else if (event.key.keysym.sym == SDLK_RETURN)
                {
                    freeText(pPlay);
                    freeText(pLvlEdit);
                    freeText(pQuit);
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
                if (r < 255)
                    r++;
                else
                    r = 0;
                break;
            case 2:
                if (g < 255)
                    g++;
                else
                    g = 0;
                break;
            case 3:
                if (b < 255)
                    b++;
                else
                    b = 0;
                break;
            }
        }
        switch (selectedMode)
        {
        case 0:
            pPlay = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            break;
        case 1:
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            break;
        case 2:
            pPlay = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Play", pGame->windowWidth / 2, pGame->windowHeight / 4);
            pLvlEdit = createText(pGame->pRenderer, 200, 200, 200, pGame->ui.pFpsFont, "Edit level", pGame->windowWidth / 2, (pGame->windowHeight / 4) + pGame->world.tileSize);
            pQuit = createText(pGame->pRenderer, r, g, b, pGame->ui.pFpsFont, "QUIT", pGame->windowWidth / 2, (pGame->windowHeight / 4) + (2 * pGame->world.tileSize));
            break;
        }

        SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pGame->pRenderer);
        drawText(pPlay, pGame->pRenderer);
        drawText(pLvlEdit, pGame->pRenderer);
        drawText(pQuit, pGame->pRenderer);
        SDL_RenderPresent(pGame->pRenderer);
    }
    freeText(pPlay);
    freeText(pLvlEdit);
    freeText(pQuit);
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
                    pMap = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, text, pGame->windowWidth / 2, pGame->windowHeight / 2);
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
void run(Game *pGame)
{
    bool exit = false;
    pGame->config.fps = 60;
    int frameCounter = 0, oneSecTimer = 0, previousTime = 0, movementPreviousTime = 0;
    while (!exit)
    {

        if (SDL_GetTicks() - oneSecTimer >= 1000) // Performance monitor
        {
            oneSecTimer = SDL_GetTicks();
            char buffer[50];
            // SDL_DestroyTexture(&pGame->ui.pFpsText->pTexture);
            sprintf(buffer, "%d", frameCounter);
            pGame->ui.pFpsText = createText(pGame->pRenderer, 0, 255, 0, pGame->ui.pFpsFont, buffer, pGame->windowWidth - pGame->world.tileSize, pGame->world.tileSize);
            printf("%s\n", buffer);
            frameCounter = 0;
        }

        int deltaTime = SDL_GetTicks() - previousTime;
        if (deltaTime >= (1000 / FPS))
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    exit = true;
            }

            int movementDeltaTime = SDL_GetTicks() - movementPreviousTime;
            if (movementDeltaTime >= (1000 / 60))
            {
                movementPreviousTime = SDL_GetTicks();
                handleInput(pGame);
                pGame->player.rect.h = (pGame->world.tileSize / 2) + ((pGame->world.tileSize / 2) * (1 - (float)pGame->player.charge / MAX_CHARGE));
                pGame->player.rect.y += pGame->world.tileSize - pGame->player.rect.h;

                if (pGame->player.hp <= 0)
                {
                    pGame->state = OVER;
                    // printf("You Died\n");
                    pGame->player.hp = 255;
                }

                pGame->ui.healthbar.w = pGame->player.hp;
                pGame->ui.chargebar.w = pGame->player.charge;
            }
            previousTime = SDL_GetTicks();
            updateScreen(pGame);
            frameCounter++;
        }
    }
}
void close(Game *pGame)
{
    if (pGame->pTileTextures[0])
        SDL_DestroyTexture(pGame->pTileTextures[0]);
    if (pGame->pTileTextures[1])
        SDL_DestroyTexture(pGame->pTileTextures[1]);
    if (pGame->pTileTextures[2])
        SDL_DestroyTexture(pGame->pTileTextures[2]);
    if (pGame->pTileTextures[3])
        SDL_DestroyTexture(pGame->pTileTextures[3]);
    if (pGame->pPlayerTexture)
        SDL_DestroyTexture(pGame->pPlayerTexture);
    if (pGame->ui.pGameFont)
        TTF_CloseFont(pGame->ui.pGameFont);
    if (pGame->ui.pFpsFont)
        TTF_CloseFont(pGame->ui.pFpsFont);
    if (pGame->ui.pMenuText)
        freeText(pGame->ui.pMenuText);
    if (pGame->ui.pOverText)
        freeText(pGame->ui.pOverText);
    if (pGame->ui.pFpsText)
        freeText(pGame->ui.pFpsText);

    if (pGame->pRenderer)
        SDL_DestroyRenderer(pGame->pRenderer);
    if (pGame->pWindow)
        SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}

// FUNKTIONER INOM RUN
void updateScreen(Game *pGame)
{
    SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pGame->pRenderer);
    SDL_Rect temp;
    for (int i = 0; i < (((pGame->player.y) / pGame->map[0].wall.w) * MAPSIZE) + ((pGame->player.x - 1) / pGame->map[0].wall.w) + 2; i++)
    {
        switch (pGame->map[i].type)
        {
        case 0:
            if (i > MAPSIZE - 1)
            {
                if (pGame->map[i - MAPSIZE].type)
                {
                    temp = pGame->map[i].wall;
                    temp.h = ((float)pGame->world.tileSize * pGame->world.angle);
                    SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type - 1)], 150, 150, 150);
                    SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[(pGame->map[i - MAPSIZE].type - 1)], NULL, &temp);
                    SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type - 1)], 255, 255, 255);
                }
            }
            break;
        case 1:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[0], NULL, &pGame->map[i].wall);
            break;
        case 2:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[1], NULL, &pGame->map[i].wall);
            break;
        case 3:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[2], NULL, &pGame->map[i].wall);
            break;
        case 4:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[3], NULL, &pGame->map[i].wall);
            break;
        default:
            break;
        }
    }
    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(pGame->pRenderer, &pGame->player.rect);
    SDL_RenderCopy(pGame->pRenderer, pGame->pPlayerTexture, NULL, &pGame->player.rect);

    for (int i = (((pGame->player.y) / pGame->map[0].wall.w) * MAPSIZE) + ((pGame->player.x - 1) / pGame->map[0].wall.w) + 2; i < MAPSIZE * MAPSIZE; i++)
    {
        switch (pGame->map[i].type)
        {
        case 0:
            if (i > MAPSIZE - 1)
            {
                if (pGame->map[i - MAPSIZE].type)
                {
                    temp = pGame->map[i].wall;
                    temp.h = ((float)pGame->world.tileSize * pGame->world.angle);
                    SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type - 1)], 150, 150, 150);
                    SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[(pGame->map[i - MAPSIZE].type - 1)], NULL, &temp);
                    SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type - 1)], 255, 255, 255);
                }
            }
            break;
        case 1:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[0], NULL, &pGame->map[i].wall);
            break;
        case 2:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[1], NULL, &pGame->map[i].wall);
            break;
        case 3:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[2], NULL, &pGame->map[i].wall);
            break;
        case 4:
            SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[3], NULL, &pGame->map[i].wall);
            break;
        default:
            break;
        }
    }
    if (pGame->state == OVER)
    {
        drawText(pGame->ui.pOverText, pGame->pRenderer);
    }
    drawText(pGame->ui.pFpsText, pGame->pRenderer);

    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 255, 0, 255);
    SDL_RenderFillRect(pGame->pRenderer, &pGame->ui.healthbar);

    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(pGame->pRenderer, &pGame->ui.chargebar);

    SDL_RenderPresent(pGame->pRenderer);
}

void centerPlayer(Game *pGame)
{
    int screenShiftAmount = pGame->movementAmount;
    if (pGame->player.rect.x >= (4 * pGame->windowWidth) / 5 || pGame->player.rect.x <= pGame->windowWidth / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pGame->player.rect.y >= (4 * pGame->windowHeight) / 5 || pGame->player.rect.y <= pGame->windowHeight / 5)
    {
        screenShiftAmount = pGame->movementAmount * 2;
    }
    if (pGame->player.rect.x >= pGame->windowWidth || pGame->player.rect.x <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pGame->player.rect.y >= pGame->windowHeight || pGame->player.rect.y <= 0)
    {
        screenShiftAmount = pGame->movementAmount * 10;
    }
    if (pGame->player.rect.y < (2 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y += screenShiftAmount;
        }
    }
    if (pGame->player.rect.y > (3 * pGame->windowHeight) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.y -= screenShiftAmount;
        }
    }
    if (pGame->player.rect.x < (2 * pGame->windowWidth) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x += screenShiftAmount;
        }
    }
    if (pGame->player.rect.x > (3 * pGame->windowWidth) / 5)
    {
        for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
        {
            pGame->map[i].wall.x -= screenShiftAmount;
        }
    }
}

int handleInput(Game *pGame)
{
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    float scaleY = (float)pGame->map[0].wall.h / pGame->world.tileSize;
    float scaleX = (float)pGame->map[0].wall.w / pGame->world.tileSize;
    if (currentKeyStates[SDL_SCANCODE_Q])
    {
        if (pGame->world.angle >= 0.01)
        {
            pGame->world.angle -= 0.01;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                pGame->map[i].wall.h = ((float)pGame->world.tileSize * (1 - pGame->world.angle));

                if (i > (MAPSIZE - 1))
                {
                    pGame->map[i].wall.y = pGame->map[i - MAPSIZE].wall.y + pGame->map[i].wall.h;
                }
            }
        }
    }
    if (currentKeyStates[SDL_SCANCODE_E])
    {
        if (pGame->world.angle < 1)
        {
            pGame->world.angle += 0.01;
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                pGame->map[i].wall.h = ((float)pGame->world.tileSize * (1 - pGame->world.angle));

                if (i > (MAPSIZE - 1))
                {
                    pGame->map[i].wall.y = pGame->map[i - MAPSIZE].wall.y + pGame->map[i].wall.h;
                }
            }
        }
    }
    if (currentKeyStates[SDL_SCANCODE_SPACE])
    {
        pGame->state = START;
        if (pGame->player.charge < MAX_CHARGE)
        {
            pGame->player.charge += 1;
        }
        else
        {
            printf("FULLY CHARGED\n");
        }
    }
    else if (pGame->player.charge > 0)
    {
        int damage = 0;

        for (int i = 0; i < 2 * (pGame->player.charge / 2); i++)
        {
            if (!checkCollision(pGame->player, pGame->map, pGame->player.prevKeyPressed, pGame->world.tileSize))
            {
                movePlayer(&pGame->player, pGame->player.prevKeyPressed);
            }
            else
            {
                damage = pGame->player.charge * 2;
                pGame->player.charge = 0;
                break;
            }
        }
        pGame->player.hp -= damage;
        pGame->player.charge -= 1;
    }
    else
    {
        for (int i = 0; i < pGame->movementAmount; i++)
        {
            if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
            {
                pGame->player.prevKeyPressed = 'W';
                if (!checkCollision(pGame->player, pGame->map, 'W', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'W');
                }
                else
                {
                    printf("COLLISION W\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
            {
                pGame->player.prevKeyPressed = 'A';
                if (!checkCollision(pGame->player, pGame->map, 'A', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'A');
                }
                else
                {
                    printf("COLLISION A\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
            {
                pGame->player.prevKeyPressed = 'S';
                if (!checkCollision(pGame->player, pGame->map, 'S', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'S');
                }
                else
                {
                    printf("COLLISION S\n");
                }
            }
            if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
            {
                pGame->player.prevKeyPressed = 'D';
                if (!checkCollision(pGame->player, pGame->map, 'D', pGame->world.tileSize))
                {
                    movePlayer(&pGame->player, 'D');
                }
                else
                {
                    printf("COLLISION D\n");
                }
            }

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
        }
    }
    // CENTER PLAYER
    centerPlayer(pGame);

    int offsetX = pGame->map[0].wall.x - pGame->map[0].x;
    int offsetY = pGame->map[0].wall.y - pGame->map[0].y;
    pGame->player.rect.x = ((float)pGame->player.x * scaleX) + offsetX;
    pGame->player.rect.y = ((float)pGame->player.y * scaleY) + offsetY;
    return 0;
}
void movePlayer(Player *pPlayer, char direction)
{
    switch (direction)
    {
    case 'W':
        pPlayer->y--;
        break;
    case 'A':
        pPlayer->x--;
        break;
    case 'S':
        pPlayer->y++;
        break;
    case 'D':
        pPlayer->x++;
        break;
    default:
        break;
    }
}
int checkCollision(Player player, Tile map[], char direction, int tileSize)
{
    switch (direction)
    {
    case 'W':
        if (map[(((player.y - 1) / tileSize) * MAPSIZE) + (player.x / tileSize)].type)
            return map[(((player.y - 1) / tileSize) * MAPSIZE) + (player.x / tileSize)].type;
        else if (map[(((player.y - 1) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type)
            return map[(((player.y - 1) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type;
        break;
    case 'A':
        if (map[(((player.y) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type)
            return map[(((player.y) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type;
        else if (map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type)
            return map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x - 1) / tileSize)].type;
        break;
    case 'S':
        if (map[(((player.y + tileSize) / tileSize) * MAPSIZE) + (player.x / tileSize)].type)
            return map[(((player.y + tileSize) / tileSize) * MAPSIZE) + (player.x / tileSize)].type;
        else if (map[(((player.y + tileSize) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type)
            return map[(((player.y + tileSize) / tileSize) * MAPSIZE) + ((player.x + (tileSize - 1)) / tileSize)].type;
        break;
    case 'D':
        if (map[(((player.y) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type)
            return map[(((player.y) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type;
        else if (map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type)
            return map[(((player.y + (tileSize - 1)) / tileSize) * MAPSIZE) + ((player.x + tileSize) / tileSize)].type;
        break;
    default:
        break;
    }

    return 0;
}
SDL_Rect findEmptyTile(Tile map[])
{
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (!map[i].type)
        {
            return map[i].wall;
        }
    }
}
void getPlayerSpawnPos(Game *pGame)
{
    SDL_Rect spawnTile = findEmptyTile(pGame->map); // this function returns a valid spawn tile
    pGame->player.x = spawnTile.x;
    pGame->player.y = spawnTile.y;
    pGame->player.rect.x = spawnTile.x; // windowWidth / 2;
    pGame->player.rect.y = spawnTile.y; // windowHeight / 2;
}

int getStringFromUser(char text[], SDL_Event event)
{
    int strCounter = strlen(text), strEnd = 0;
    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_RETURN)
        {
            if(strCounter){
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