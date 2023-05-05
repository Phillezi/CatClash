#include "definitions.h"
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "init.h"
#include "text.h"
#include "menu.h"
#include "player.h"
#include "levelEditor.h"
#include "pthread.h"
#include "newClient.h"
#include <time.h>

int init(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void *updateScreen(void *pGameIn);

int main(int argv, char **args)
{
    Game game;
    // game.serverThread;
    char mapName[31];

    // game.serverIsHosted = false;

    if (init(&game))
    {
        close(&game);
        return 1;
    }
    while (1)
    {
        switch (mainMenu(&game))
        {
        case PLAY:
            if (testSelectMenu(&game, mapName))
                break;
            run(&game);
            break;
        case EDIT:
            if (testSelectMenu(&game, mapName))
                break;
            levelEditor(&game);
            break;
        case QUIT:
            printf("Closing...\n");
            /*
            if (game.serverIsHosted)
            {
                printf("Closing server...\n");
                pthread_cancel(game.serverThread);
                pthread_join(game.serverThread, NULL);
                game.serverIsHosted = false;
            }
            */
            printf("Closing game...\n");
            close(&game);
            printf("Done closing!\n");
            return 0;
            break;
        case JOIN:
            if (joinServerMenu(&game))
                break;
            run(&game);
            break;
        case CATSEL:
            if (catSelMenu(&game))
                break;
            break;
        case HOST:
            break;
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
            break;
        default:
            break;
        }
    }
}

int init(Game *pGame)
{
    pGame->isConnected = false;
    char windowTitle[100] = "CatClash   |";
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != 0)
    {
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }
    if (SDLNet_Init())
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return 1;
    }

    pGame->config.vSync = false; // Hårdkodad

    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
    {
        printf("SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
        return 1;
    }

    pGame->windowWidth = (float)displayMode.w * 0.3; // 70% of avaliable space
    pGame->windowHeight = (float)displayMode.h * 0.3;

    // pGame->windowWidth = 1920; // 70% of avaliable space
    // pGame->windowHeight = 1080;

    pGame->world.tileSize = (pGame->windowHeight / MAPSIZE) * 4;

    pGame->pPlayer = createPlayer(0, "Name", pGame->world.tileSize);

    if (!pGame->pPlayer)
    {
        printf("Error: Failed to create player\n");
        return 1;
    }

    pGame->pWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pGame->windowWidth, pGame->windowHeight, 0);
    if (!pGame->pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    if (pGame->config.vSync)
    {
        pGame->pRenderer = SDL_CreateRenderer(pGame->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        printf("Vsync is enabled\n");
        sprintf(windowTitle, "%s Vsync |", windowTitle);
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

    loadTileAtlas(pGame->pRenderer, pGame->pTileTextures, "resources/tileMap.png");
    if (!pGame->pTileTextures[0])
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

    pGame->ui.pNameTagFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", (int)((float)pGame->world.tileSize / 4));
    if (!pGame->ui.pNameTagFont)
    {
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }

    pGame->ui.pWinText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "YOU WIN!!", pGame->windowWidth / 2, pGame->windowHeight / 5);
    pGame->ui.pMenuText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "Use <- -> To Spectate", pGame->windowWidth / 2, pGame->windowHeight - pGame->windowHeight / 5);
    pGame->ui.pOverText = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, "You Died!", pGame->windowWidth / 2, pGame->windowHeight / 5);
    if (!pGame->ui.pMenuText || !pGame->ui.pOverText)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    if (SDL_QueryTexture(pGame->pPlayerTexture, NULL, NULL, &pGame->pPlayer->rect.w, &pGame->pPlayer->rect.h) < 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    getPlayerSpawnPos(pGame);

    pGame->pPlayer->rect.w = pGame->world.tileSize;
    pGame->pPlayer->rect.h = pGame->world.tileSize;

    pGame->pPlayer->hp = 255;
    pGame->pPlayer->state = ALIVE;

    pGame->ui.chargebar.x = ((pGame->windowWidth / 2) - (MAX_CHARGE / 2));
    pGame->ui.chargebar.y = ((3 * pGame->windowHeight) / 4);
    pGame->ui.chargebar.w = pGame->pPlayer->charge;
    pGame->ui.chargebar.h = pGame->world.tileSize;

    pGame->ui.healthbar.x = ((pGame->windowWidth / 2) + (MAX_CHARGE / 2) + 5);
    pGame->ui.healthbar.y = ((3 * pGame->windowHeight) / 4);
    pGame->ui.healthbar.w = pGame->pPlayer->hp;
    pGame->ui.healthbar.h = pGame->world.tileSize;

    pGame->movementAmount = (float)pGame->world.tileSize / TILESIZE;

    pGame->config.multiThreading = true;

    if (pGame->config.multiThreading)
    {
        printf("Multithreading is enabled\n");
        sprintf(windowTitle, "%s MultiThreaded |", windowTitle);
    }
    else
    {
        printf("Multithreading is not enabled\n");
    }

    SDL_SetWindowTitle(pGame->pWindow, windowTitle);
    SDL_Surface *pSurface = IMG_Load("resources/caet2.png");
    if (!pSurface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return 1;
    }
    SDL_SetWindowIcon(pGame->pWindow, pSurface);
    SDL_FreeSurface(pSurface);

    loadMedia(pGame->pRenderer, &pGame->pPlayerTexture, pGame->gSpriteClips, pGame->pPlayer->id);
    pGame->pPlayer->idle = 1;
    pGame->pPlayer->charging = 0;

    pGame->tempID = 0;

    return 0;
}

void run(Game *pGame)
{
    bool playerWasConnected = false;
    findPortal(pGame);
    char windowTitle[31];
    sprintf(windowTitle, "CLIENT: %d", pGame->pPlayer->id);
    SDL_SetWindowTitle(pGame->pWindow, windowTitle);
    if (pGame->isConnected)
    {
        pGame->pPacket = SDLNet_AllocPacket(sizeof(PlayerUdpPkg));
        if (pGame->socketDesc = SDLNet_UDP_Open(0))
        {
            printf("UDP init\n");
        }
    }

    // if(pGame->config.multiThreading)
    // pthread_t renderThread;
    int oldX = 0;
    int oldY = 0;
    int oldCharge = 0;
    int prevUDPTransfer = 0;
    pthread_t movementThread;
    bool exit = false;
    pGame->config.fps = 60;
    int frameCounter = 0, oneSecTimer = 0, previousTime = 0, movementPreviousTime = 0;
    while (!exit)
    {
        if (SDL_GetTicks() - oneSecTimer >= 1000) // Performance monitor
        {
            oneSecTimer = SDL_GetTicks();
            char buffer[50];
            if (pGame->ui.pFpsText)
                freeText(pGame->ui.pFpsText);
            sprintf(buffer, "%d", frameCounter);
            pGame->ui.pFpsText = createText(pGame->pRenderer, 0, 255, 0, pGame->ui.pFpsFont, buffer, pGame->windowWidth - pGame->world.tileSize, pGame->world.tileSize);
            // printf("%s\n", buffer);
            frameCounter = 0;
        }

        int deltaTime = SDL_GetTicks() - previousTime;
        if (deltaTime >= (1000 / FPS))
        {
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
            /*
            if (pGame->config.multiThreading){
                pthread_create(&renderThread, NULL, updateScreen, (void *)pGame);
            }
            */

            int movementDeltaTime = SDL_GetTicks() - movementPreviousTime;
            if (movementDeltaTime >= (1000 / 60))
            {
                movementPreviousTime = SDL_GetTicks();
                if (pGame->config.multiThreading)
                {
                    static int idle = 0;
                    if (pGame->isConnected)
                        getPlayerData(pGame);
                    pthread_join(movementThread, NULL);
                    int keepAliveDelta = SDL_GetTicks() - prevUDPTransfer;
                    if (pGame->isConnected)
                    {
                        if (oldX != pGame->pPlayer->x || oldY != pGame->pPlayer->y || oldCharge != pGame->pPlayer->charge || keepAliveDelta > 4500)
                        {
                            prevUDPTransfer = SDL_GetTicks();
                            oldX = pGame->pPlayer->x;
                            oldY = pGame->pPlayer->y;
                            oldCharge = pGame->pPlayer->charge;
                            // printf("Trying to send data\n");
                            sendData(pGame);
                            idle = 1;
                        }
                        else if (idle)
                        {
                            sendData(pGame);
                            idle = 0;
                        } // Send one last data packet så other players know you are idle
                    }

                    pthread_create(&movementThread, NULL, handleInput, (void *)pGame);
                }
                else
                {
                    if (pGame->isConnected)
                        getPlayerData(pGame);
                    handleInput(pGame);
                    int keepAliveDelta = SDL_GetTicks() - prevUDPTransfer;
                    if (pGame->isConnected)
                    {
                        if (oldX != pGame->pPlayer->x || oldY != pGame->pPlayer->y || oldCharge != pGame->pPlayer->charge || keepAliveDelta > 4500)
                        {
                            prevUDPTransfer = SDL_GetTicks();
                            oldX = pGame->pPlayer->x;
                            oldY = pGame->pPlayer->y;
                            oldCharge = pGame->pPlayer->charge;
                            // printf("Trying to send data\n");
                            sendData(pGame);
                        }
                    }
                }

                if (pGame->pPlayer->hp <= 0 && pGame->pPlayer->state == ALIVE)
                {
                    pGame->pPlayer->state = DEAD;
                }

                if (getAlivePlayers(pGame) == 0 && getDeadPlayers(pGame) >= 1)
                {
                    if (pGame->pPlayer->state == ALIVE)
                    {
                        pGame->pPlayer->state = WIN;
                    }
                }

                pGame->ui.healthbar.w = pGame->pPlayer->hp;
                pGame->ui.chargebar.w = pGame->pPlayer->charge;
            }
            previousTime = SDL_GetTicks();
            // if (pGame->config.multiThreading)
            //  pthread_join(renderThread, NULL);
            // else
            updateScreen(pGame);
            frameCounter++;
        }
        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_ESCAPE])
        {
            exit = true;
            break;
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                break;
            }
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
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
                    if (pGame->tempID >= pGame->nrOfPlayers - 1)
                        pGame->tempID = 0;
                    else
                        pGame->tempID += 1;

                    while (pGame->pMultiPlayer[pGame->tempID].state == DEAD)
                        if (pGame->tempID >= pGame->nrOfPlayers - 1)
                            pGame->tempID = 0;
                        else
                            pGame->tempID += 1;
                }
                else if (event.key.keysym.sym == SDLK_LEFT)
                {
                    if (pGame->tempID <= 0)
                        pGame->tempID = pGame->nrOfPlayers - 1;
                    else
                        pGame->tempID -= 1;

                    while (pGame->pMultiPlayer[pGame->tempID].state == DEAD)
                        if (pGame->tempID <= 0)
                            pGame->tempID = pGame->nrOfPlayers - 1;
                        else
                            pGame->tempID -= 1;
                }
            }
        }
    }
    if (pGame->isConnected)
    {
        SDLNet_TCP_Close(pGame->pClient->socketTCP);
        SDLNet_FreeSocketSet(pGame->pClient->sockets);
        free(pGame->pClient);
    }
}

void close(Game *pGame)
{
    if (pGame->pMultiPlayer)
        free(pGame->pMultiPlayer);
    if (pGame->pPacket)
    {
        SDLNet_FreePacket(pGame->pPacket);
    }
    if (pGame->socketDesc)
    {
        SDLNet_UDP_Close(pGame->socketDesc);
    }
    if (pGame->pPlayer)
    {
        destroyPlayer(pGame->pPlayer);
    }
    for (int i = 0; i < TILES; i++)
    {
        if (pGame->pTileTextures[i])
            SDL_DestroyTexture(pGame->pTileTextures[i]);
    }
    /* if (pGame->pTileTextures[1])
         SDL_DestroyTexture(pGame->pTileTextures[1]);
     if (pGame->pTileTextures[2])
         SDL_DestroyTexture(pGame->pTileTextures[2]);
     if (pGame->pTileTextures[3])
         SDL_DestroyTexture(pGame->pTileTextures[3]);*/
    if (pGame->pPlayerTexture)
        SDL_DestroyTexture(pGame->pPlayerTexture);
    if (pGame->ui.pNameTagFont)
        TTF_CloseFont(pGame->ui.pNameTagFont);
    if (pGame->ui.pGameFont)
        TTF_CloseFont(pGame->ui.pGameFont);
    if (pGame->ui.pFpsFont)
        TTF_CloseFont(pGame->ui.pFpsFont);
    if (pGame->ui.pMenuText)
        freeText(pGame->ui.pMenuText);
    if (pGame->ui.pOverText)
        freeText(pGame->ui.pOverText);
    if (pGame->ui.pWinText)
        freeText(pGame->ui.pWinText);
    if (pGame->ui.pFpsText)
        freeText(pGame->ui.pFpsText);
    TTF_Quit();

    SDLNet_Quit();

    if (pGame->pRenderer)
        SDL_DestroyRenderer(pGame->pRenderer);
    if (pGame->pWindow)
        SDL_DestroyWindow(pGame->pWindow);
    SDL_Quit();
}

// FUNKTIONER INOM RUN
void *updateScreen(void *pGameIn)
{
    Game *pGame = (Game *)pGameIn;

    SDL_Rect backGround;
    backGround.x = pGame->map[0].wall.x;
    backGround.y = pGame->map[0].wall.y + (pGame->world.tileSize - pGame->map[0].wall.h);
    backGround.w = MAPSIZE * pGame->map[0].wall.w;
    backGround.h = MAPSIZE * pGame->map[0].wall.h;
    SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pGame->pRenderer);
    SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[19], NULL, &backGround);
    SDL_Rect temp;

    translatePositionToScreen(pGame);

    // Checking if other players charge into you
    int id = 0, check = 0, collision = 0;
    static int count = 300;
    static char dir[4] = {'W', 'A', 'S', 'D'};

    for (int i = 0; i < pGame->nrOfPlayers; i++)
    {
        if (pGame->pMultiPlayer[i].charging == 1)
            check = 1;
    }

    if (count == 300)
    {
        if (check)
        {
            for (int i = 0; i < 4; i++)
            {
                if ((id = playerCollision(*pGame->pPlayer, pGame->pMultiPlayer, pGame->nrOfPlayers, dir[i], pGame->world.tileSize)) != -1)
                    if (pGame->pMultiPlayer[id].charging)
                    {
                        collision = 1;
                        break;
                    }
            }
            if (collision)
            {
                printf("player charge: %d\tcolliding charge: %d\n", pGame->pPlayer->charge, pGame->pMultiPlayer[id].charge);
                if (pGame->pPlayer->charge < pGame->pMultiPlayer[id].charge)
                {
                    pGame->pPlayer->hp -= (pGame->pMultiPlayer[id].charge * 2);
                    count = 0;
                }
            }
        }
    }
    else
        count++;

    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (((pGame->map[i].wall.x <= pGame->windowWidth) && (pGame->map[i].wall.x + pGame->world.tileSize >= 0)) && ((pGame->map[i].wall.y <= pGame->windowHeight) && (pGame->map[i].wall.y + pGame->world.tileSize >= 0)))
        {
            if (pGame->map[i].type > 0)
                SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[pGame->map[i].type], NULL, &pGame->map[i].wall);
            else
            {
                // SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[19], NULL, &pGame->map[i].wall);
                if (i > MAPSIZE - 1)
                {
                    if (pGame->map[i - MAPSIZE].type == 1)
                    {
                        temp = pGame->map[i].wall;
                        temp.h = ((float)pGame->world.tileSize * pGame->world.angle);
                        SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type)], 150, 150, 150);
                        SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[(pGame->map[i - MAPSIZE].type)], NULL, &temp);
                        SDL_SetTextureColorMod(pGame->pTileTextures[(pGame->map[i - MAPSIZE].type)], 255, 255, 255);
                    }
                    else if (pGame->map[i - MAPSIZE].type > 0)
                    {
                        temp = pGame->map[i].wall;
                        temp.h = ((float)pGame->world.tileSize * pGame->world.angle);
                        SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[2], NULL, &temp);
                    }
                }
            }
            if (i == (((pGame->pPlayer->y / pGame->map[0].wall.w) * MAPSIZE) + ((pGame->pPlayer->x - 1) / pGame->map[0].wall.w) + 2))
            {
                loadMedia(pGame->pRenderer, &pGame->pPlayerTexture, pGame->gSpriteClips, pGame->pPlayer->id);
                drawPlayer(pGame, *pGame->pPlayer, pGame->pPlayer->id);
            }
            for (int j = 0; j < pGame->nrOfPlayers; j++)
            {
                if (i == (((pGame->pMultiPlayer[j].y / pGame->map[0].wall.w) * MAPSIZE) + ((pGame->pMultiPlayer[j].x - 1) / pGame->map[0].wall.w) + 2))
                {
                    loadMedia(pGame->pRenderer, &pGame->pPlayerTexture, pGame->gSpriteClips, pGame->pMultiPlayer[j].id);
                    drawPlayer(pGame, pGame->pMultiPlayer[j], pGame->pMultiPlayer[j].id);
                    pGame->ui.pPlayerName = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pNameTagFont, pGame->pMultiPlayer[j].name, pGame->pMultiPlayer[j].rect.x + (pGame->pMultiPlayer[j].rect.w / 2), pGame->pMultiPlayer[j].rect.y);
                    drawText(pGame->ui.pPlayerName, pGame->pRenderer);
                    freeText(pGame->ui.pPlayerName);
                }
            }
        }
    }

    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);

    /*
    // Draw players
    drawPlayer(pGame, *pGame->pPlayer, pGame->pPlayer->id);
    for (int i = 0; i < pGame->nrOfPlayers; i++)
    {
        char buffer[31];
        sprintf(buffer, "P: %d", pGame->pMultiPlayer[i].id);
        drawPlayer(pGame, pGame->pMultiPlayer[i], pGame->pMultiPlayer[i].id);
        pGame->ui.pPlayerName = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pFpsFont, buffer, pGame->pMultiPlayer[i].rect.x, pGame->pMultiPlayer[i].rect.y + (pGame->world.tileSize / 2));
        drawText(pGame->ui.pPlayerName, pGame->pRenderer);
        freeText(pGame->ui.pPlayerName);
    }
    */

    if (pGame->pPlayer->state == DEAD)
    {
        drawText(pGame->ui.pOverText, pGame->pRenderer);
        drawText(pGame->ui.pMenuText, pGame->pRenderer);
    }

    if (pGame->pPlayer->state == WIN)
    {
        drawText(pGame->ui.pWinText, pGame->pRenderer);
    }

    drawText(pGame->ui.pFpsText, pGame->pRenderer);

    if (pGame->pPlayer->state == ALIVE)
    {
        SDL_SetRenderDrawColor(pGame->pRenderer, 255 - pGame->pPlayer->hp, pGame->pPlayer->hp, 0, 255);
        SDL_RenderFillRect(pGame->pRenderer, &pGame->ui.healthbar);

        SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);
        SDL_RenderFillRect(pGame->pRenderer, &pGame->ui.chargebar);
    }

    SDL_RenderPresent(pGame->pRenderer);
}
