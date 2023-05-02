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
// #include "client.h"
#include "clientUDP.h"
#include "multiThreadedServer.h"

int init(Game *pGame);
void run(Game *pGame);
void close(Game *pGame);
void *updateScreen(void *pGameIn);

int main(int argv, char **args)
{
    Game game;
    pthread_t serverThread;
    char mapName[31];
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
            // if (mapSelection(&game))
            //     break;
            if (testSelectMenu(&game, mapName))
                break;
            run(&game);
            break;
        case EDIT:
            // if (mapSelection(&game))
            //     break;
            if (testSelectMenu(&game, mapName))
                break;
            levelEditor(&game);
            break;
        case QUIT:
            if (serverThread)
            {
                pthread_cancel(serverThread);
                pthread_join(serverThread, NULL);
            }
            close(&game);
            return 0;
            break;
        case JOIN:
            if (joinServerMenu(&game))
                break;
            run(&game);
            break;
        case CATSEL:
            if(catSelMenu(&game))
                break;
                break;   
        case HOST:
            if (testSelectMenu(&game, mapName))
                break;
            pthread_create(&serverThread, NULL, MThostServer, (void *)mapName);
            break;
        default:
            break;
        }
    }
}

int init(Game *pGame)
{
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

    pGame->windowWidth = (float)displayMode.w * 0.4; // 70% of avaliable space
    pGame->windowHeight = (float)displayMode.h * 0.4;

    pGame->world.tileSize = (pGame->windowHeight / MAPSIZE) * 4;

    pGame->pPlayer = createPlayer(0, "test", pGame->world.tileSize);

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
    /*
        char tileTextures[TILES][20] = {"resources/Tile1.png", "resources/Tile2.png", "resources/Tile3.png", "resources/Tile4.png"};
        if (initTextureTiles(pGame->pRenderer, pGame->pWindow, pGame->pTileTextures, tileTextures, TILES) == -1)
        {
            printf("Error: %s\n", SDL_GetError());
            return 1;
        }
    */
    loadTileAtlas(pGame->pRenderer, pGame->pTileTextures, "resources/texturedemo.png");
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

    pGame->ui.pMenuText = createText(pGame->pRenderer, 97, 181, 97, pGame->ui.pGameFont, "Press Space to play!", pGame->windowWidth / 2, pGame->windowHeight / 2);
    pGame->ui.pOverText = createText(pGame->pRenderer, 20, 197, 204, pGame->ui.pGameFont, "You Died!", pGame->windowWidth / 2, pGame->windowHeight / 2);
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
    
    loadMedia(pGame->pRenderer, &pGame->pPlayerTexture, pGame->gSpriteClips);
    pGame->pPlayer->idle = 1;    
    return 0;
}

void run(Game *pGame)
{
    pGame->pPacket = SDLNet_AllocPacket(sizeof(PlayerUdpPkg));
    if (pGame->socketDesc = SDLNet_UDP_Open(0))
    {
        printf("UDP init\n");
    }
    // if(pGame->config.multiThreading)
    // pthread_t renderThread;
    int oldX = 0;
    int oldY = 0;
    int oldCharge = 0;
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
                    getPlayerData(pGame, pGame->players);
                    pthread_join(movementThread, NULL);
                    if (oldX != pGame->pPlayer->x || oldY != pGame->pPlayer->y || oldCharge != pGame->pPlayer->charge)
                    {
                        oldX = pGame->pPlayer->x;
                        oldY = pGame->pPlayer->y;
                        oldCharge = pGame->pPlayer->charge;
                        // printf("Trying to send data\n");
                        sendData(pGame);
                        idle = 1;
                    }
                    else if (idle) { sendData(pGame); idle = 0; } // Send one last data packet så other players know you are idle
                    pthread_create(&movementThread, NULL, handleInput, (void *)pGame);
                }
                else
                {
                    getPlayerData(pGame, pGame->players);
                    handleInput(pGame);
                    if (oldX != pGame->pPlayer->x || oldY != pGame->pPlayer->y || oldCharge != pGame->pPlayer->charge)
                    {
                        oldX = pGame->pPlayer->x;
                        oldY = pGame->pPlayer->y;
                        oldCharge = pGame->pPlayer->charge;
                        // printf("Trying to send data\n");
                        sendData(pGame);
                    }
                }

                if (pGame->pPlayer->hp <= 0)
                {
                    pGame->state = OVER;
                    // printf("You Died\n");
                    pGame->pPlayer->hp = 255;
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
            }
        }
    }
}

void close(Game *pGame)
{
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
    SDL_SetRenderDrawColor(pGame->pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pGame->pRenderer);
    SDL_Rect temp;
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (((pGame->map[i].wall.x <= pGame->windowWidth) && (pGame->map[i].wall.x + pGame->world.tileSize >= 0)) && ((pGame->map[i].wall.y <= pGame->windowHeight) && (pGame->map[i].wall.y + pGame->world.tileSize >= 0)))
        {
            if (pGame->map[i].type > 0)
                SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[pGame->map[i].type - 1], NULL, &pGame->map[i].wall);
            else
            {
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
            }
        }
    }
    
    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);

    translatePositionToScreen(pGame);
    // Draw players
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pGame->players[i].x != 0) drawPlayer(pGame, pGame->players[i], i);
        else drawPlayer(pGame, *pGame->pPlayer, i);
    }

    if (pGame->state == OVER)
    {
        drawText(pGame->ui.pOverText, pGame->pRenderer);
    }
    drawText(pGame->ui.pFpsText, pGame->pRenderer);

    SDL_SetRenderDrawColor(pGame->pRenderer, 255 - pGame->pPlayer->hp, pGame->pPlayer->hp, 0, 255);
    SDL_RenderFillRect(pGame->pRenderer, &pGame->ui.healthbar);

    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(pGame->pRenderer, &pGame->ui.chargebar);

    SDL_RenderPresent(pGame->pRenderer);
}
