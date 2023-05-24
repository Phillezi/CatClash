#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "pthread.h"
#include "../include/definitions.h"
#include "../include/init.h"
#include "../include/text.h"
#include "../include/menu.h"
#include "../include/player.h"
#include "../include/levelEditor.h"
#include "../include/newClient.h"

int init(Game *pGame);
#ifdef SDL_MIXER_H_
bool loadMusic(Game *pGame);
#endif
void run(Game *pGame);
void closeG(Game *pGame);
void *updateScreen(void *pGameIn);

Game *createGame()
{
    Game *pNew_game = (Game *)malloc(sizeof(Game));
    pNew_game->pPlayer = NULL;
    pNew_game->pMultiPlayer = NULL;
    pNew_game->pWindow = NULL;
    pNew_game->pRenderer= NULL;
    pNew_game->pPlayerTexture = NULL;
    pNew_game->pPacket = NULL;
    pNew_game->pClient = NULL;
    for(int i = 0; i < TILES; i++)
        pNew_game->pTileTextures[i] = NULL;

    // Set variables
    pNew_game->isConnected = false;
    pNew_game->packetAllocatedFlag = 0;
    pNew_game->ui.playerTookDamage = false;
    pNew_game->isDrawing = false;
    pNew_game->nrOfPlayers = 0;
    pNew_game->nrOfPlayersAlive = 0;
    pNew_game->state = START;

    // UI stuff
    pNew_game->ui.playerTookDamage = false;
    pNew_game->ui.damageRedChannel = 0;
    pNew_game->ui.pMenuText = NULL;
    pNew_game->ui.pOverText = NULL;
    pNew_game->ui.pFpsText = NULL;
    pNew_game->ui.pPlayerName = NULL;
    pNew_game->ui.pWinText = NULL;
    pNew_game->ui.pGameFont = NULL;
    pNew_game->ui.pFpsFont = NULL;
    pNew_game->ui.pNameTagFont = NULL;

    #ifdef SDL_MIXER_H_
    pNew_game->pMusic = NULL;
    pNew_game->pCharge = NULL;
    pNew_game->pHit = NULL;
    pNew_game->pBonk = NULL;
    pNew_game->pWin = NULL;
    pNew_game->pMenuSwitch = NULL;
    #endif

    return pNew_game;
}

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
    if (init(pGame))
    {
        closeG(pGame);
        free(pGame);
        return 1;
    }
    while (1)
    {
        switch (mainMenu(pGame))
        {
        case PLAY:
            if (testSelectMenu(pGame, mapName))
                break;
            run(pGame);
            break;
        case EDIT:
            if (testSelectMenu(pGame, mapName))
                break;
            levelEditor(pGame);
            break;
        case QUIT:
            printf("Closing...\n");

            printf("Closing game...\n");
            closeG(pGame);
            free(pGame);
            printf("Done closing!\n");
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
                    run(pGame);
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
    closeG(pGame);
    free(pGame);
    free(mapName);
    return 0;
}

int init(Game *pGame)
{
    sem_init(&pGame->pGameSemaphore, 0, 1);
    printf("\n\n\nLoading.");
    char windowTitle[100] = "CatClash   |";
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }
    printf(".");
    if (TTF_Init() != 0)
    {
        printf("Error: %s\n", TTF_GetError());
        return 1;
    }
    printf(".");
    if (SDLNet_Init())
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return 1;
    }
    printf(".");

    #ifdef SDL_MIXER_H_
    // Initialize SDL_mixer
    printf(".");

    printf(".");
    if (Mix_Init(MIX_INIT_OGG | MIX_INIT_FLAC | MIX_INIT_MID | MIX_INIT_MOD | MIX_INIT_MP3) == 0)
    {
        printf("Mix_Init: %s\n", Mix_GetError());
        return 1;
    }
    printf(".");
    if (Mix_OpenAudioDevice(22050, MIX_DEFAULT_FORMAT, 2, 4096, NULL, SDL_AUDIO_ALLOW_FORMAT_CHANGE | SDL_AUDIO_ALLOW_SAMPLES_CHANGE) == -1)
    {
        printf("Mix_OpenAudio: %s\n", Mix_GetError());
        return 1;
    }
    printf(".");

    if (!loadMusic(pGame))
    {
        printf("Failed to load music\n");
        return 1;
    }
    else if (Mix_PlayingMusic() == 0)
    {
        printf(".");
        if (Mix_PlayMusic(pGame->pMusic, -1) == -1)
        {
            printf("Failed to play music\n");
            return 1;
        }
    }
    printf(".");
    
    #endif
    printf("\nDone Initializing Libs\n");
    if (readConfig(pGame)) // couldnt read config
    {
        pGame->config.fps = 60;
        pGame->config.vSync = false;
        pGame->config.multiThreading = true;
        pGame->config.volumeMusic = 15;

        SDL_DisplayMode displayMode;
        if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
        {
            printf("SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
            return 1;
        }

        pGame->windowWidth = (float)displayMode.w * 0.3; // 70% of avaliable space
        pGame->windowHeight = (float)displayMode.h * 0.3;
    }
    pGame->world.tileSize = (pGame->windowHeight / MAPSIZE) * 4;
    pGame->pPlayer = createPlayer(0, "Name", pGame->world.tileSize);
    if (!pGame->pPlayer)
    {
        printf("Error: Failed to create player\n");
        return 1;
    }

    #ifdef SDL_MIXER_H_
    setVolume(pGame);
    #endif

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

    pGame->ui.pWinText = createText(pGame->pRenderer, 255, 255, 255, pGame->ui.pFpsFont, "YOU WIN!!", pGame->windowWidth / 2, pGame->windowHeight / 5);
    pGame->ui.pMenuText = createText(pGame->pRenderer, 255, 255, 255, pGame->ui.pFpsFont, "Use <- -> To Spectate", pGame->windowWidth / 2, pGame->windowHeight - pGame->windowHeight / 5);
    pGame->ui.pOverText = createText(pGame->pRenderer, 255, 255, 255, pGame->ui.pFpsFont, "You Died!", pGame->windowWidth / 2, pGame->windowHeight / 5);
    if (!pGame->ui.pMenuText || !pGame->ui.pOverText || !pGame->ui.pWinText)
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

    pGame->movementAmount = pGame->world.tileSize / TILESIZE;

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

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        loadMedia(pGame->pRenderer, &pGame->pPlayerTexture, pGame->gSpriteClips, i);
    }

    pGame->pPlayer->idle = 1;
    pGame->pPlayer->charging = 0;
    pGame->tempID = 0;

    return 0;
}

#ifdef SDL_MIXER_H_
bool loadMusic(Game *pGame)
{
    // Load background music
    pGame->pMusic = Mix_LoadMUS("resources/music/catTheme.wav");
    if (pGame->pMusic == NULL)
    {
        printf("Failed to load background music: %s\n", Mix_GetError());
        return 0;
    }

    // Load sound effects
    pGame->pCharge = Mix_LoadWAV("resources/music/charging.wav");
    if((pGame->pCharge == NULL))
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        return 0;
    }
    pGame->pHit = Mix_LoadWAV("resources/music/hit.wav");
    if((pGame->pHit == NULL))
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        return 0;
    }
    pGame->pBonk = Mix_LoadWAV("resources/music/bonk.wav");
    if((pGame->pBonk == NULL))
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        return 0;
    }
    pGame->pWin = Mix_LoadWAV("resources/music/win.wav");
    if((pGame->pWin == NULL))
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        return 0;
    }
    pGame->pMenuSwitch = Mix_LoadWAV("resources/music/menuSwitch.wav");
    if ((pGame->pMenuSwitch == NULL))
    {
        printf("Failed to load sound effects: %s\n", Mix_GetError());
        return 0;
    }

    // Successfully loaded music
    return 1;
}
#endif

void run(Game *pGame)
{
    bool playerWasConnected = false;
    findPortal(pGame);
    char windowTitle[31];
    sprintf(windowTitle, "CLIENT: %d", pGame->pPlayer->id);
    SDL_SetWindowTitle(pGame->pWindow, windowTitle);

    int oldX = 0;
    int oldY = 0;
    int oldCharge = 0;
    int prevUDPTransfer = 0;
    int oldHealth = 0;
    pthread_t movementThread;
    bool exit = false;
    int frameCounter = 0, oneSecTimer = 0, previousTime = 0, movementPreviousTime = 0;
    int previousPlayerHP = pGame->pPlayer->hp;
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
            frameCounter = 0;
        }

        int deltaTime = SDL_GetTicks() - previousTime;
        if (deltaTime >= (1000 / pGame->config.fps))
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

            int movementDeltaTime = SDL_GetTicks() - movementPreviousTime;
            if (movementDeltaTime >= (1000 / 60))
            {
                movementPreviousTime = SDL_GetTicks();

                if (pGame->config.multiThreading)
                {
                    static int idle = 0;
                    pthread_join(movementThread, NULL);
                    int keepAliveDelta = SDL_GetTicks() - prevUDPTransfer;
                    if (pGame->isConnected)
                    {
                        if (oldX != pGame->pPlayer->x || oldY != pGame->pPlayer->y || oldCharge != pGame->pPlayer->charge || oldHealth != pGame->pPlayer->hp || keepAliveDelta > 4500)
                        {
                            prevUDPTransfer = SDL_GetTicks();
                            oldX = pGame->pPlayer->x;
                            oldY = pGame->pPlayer->y;
                            oldCharge = pGame->pPlayer->charge;
                            oldHealth = pGame->pPlayer->hp;
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
                    if (pGame->isConnected)
                        getPlayerData(pGame);
                    
                    if (oldCharge != 0 && pGame->pPlayer->charge == 0)
                    {
                        #ifdef SDL_MIXER_H_
                        Mix_PlayChannel(-1, pGame->pBonk, 0);
                        #else
                        ;
                        #endif
                    }
                    

                    pthread_create(&movementThread, NULL, handleInput, (void *)pGame);
                }
                else
                {
                    static int idle = 0;
                    if (pGame->isConnected)
                        getPlayerData(pGame);
                    if (oldCharge != 0 && pGame->pPlayer->charge == 0)
                    {
#ifdef SDL_MIXER_H_
                        Mix_PlayChannel(-1, pGame->pBonk, 0);
#endif
                    }
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
                            idle = 1;
                        }
                        else if (idle)
                        {
                            sendData(pGame);
                            idle = 0;
                        } 
                    }
                }
                if (pGame->pPlayer->hp < previousPlayerHP)
                {
                    pGame->ui.playerTookDamage = true;
                    previousPlayerHP = pGame->pPlayer->hp;
#ifdef SDL_MIXER_H_
                    Mix_PlayChannel(-1, pGame->pHit, 0);
#endif
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
#ifdef SDL_MIXER_H_
                        Mix_PlayChannel(-1, pGame->pWin, 0);
#endif
                    }
                }

                pGame->ui.healthbar.w = pGame->pPlayer->hp;
                pGame->ui.chargebar.w = pGame->pPlayer->charge;
            }
            previousTime = SDL_GetTicks();

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

            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_RIGHT)
                {
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
                }
            }
        }
    }
    /*if (pGame->isConnected)
    {
        SDLNet_TCP_Close(pGame->pClient->socketTCP);
        SDLNet_FreeSocketSet(pGame->pClient->sockets);
        free(pGame->pClient);
    }*/
}

void closeG(Game *pGame)
{

    if (pGame->pMultiPlayer)
    {
        printf("Freeing memory of: pMultiplayer\n");
        free(pGame->pMultiPlayer);
    }

    if (pGame->pPacket)
    {
        printf("Freeing memory of: pPacket\n");
        SDLNet_FreePacket(pGame->pPacket);
    }
    if (pGame->socketDesc)
    {
        printf("Freeing memory of: SocketDesct\n");
        SDLNet_UDP_Close(pGame->socketDesc);
    }
    if (pGame->pPlayer)
    {
        printf("Freeing memory of: pPlayer\n");
        destroyPlayer(pGame->pPlayer);
    }
    for (int i = 0; i < TILES; i++)
    {
        if (pGame->pTileTextures[i])
        {
            printf("Freeing memory of: pTileTexture[%d]\n", i);
            SDL_DestroyTexture(pGame->pTileTextures[i]);
        }
    }

    if (pGame->pPlayerTexture)
    {
        printf("Freeing memory of: pPlayerTexture\n");
        SDL_DestroyTexture(pGame->pPlayerTexture);
    }

    if (pGame->ui.pMenuText)
    {
        printf("Freeing memory of: pMenuText\n");
        freeText(pGame->ui.pMenuText);
    }

    if (pGame->ui.pOverText)
    {
        printf("Freeing memory of: pOverText\n");
        freeText(pGame->ui.pOverText);
    }

    if (pGame->ui.pWinText)
    {
        printf("Freeing memory of: pWinText\n");
        freeText(pGame->ui.pWinText);
    }

    if (pGame->ui.pFpsText)
    {
        printf("Freeing memory of: pFpsText\n");
        freeText(pGame->ui.pFpsText);
    }

    if (pGame->ui.pNameTagFont)
    {
        printf("Freeing memory of: pNameTagFont\n");
        TTF_CloseFont(pGame->ui.pNameTagFont);
    }

    if (pGame->ui.pGameFont)
    {
        printf("Freeing memory of: pGameFont\n");
        TTF_CloseFont(pGame->ui.pGameFont);
    }

    if (pGame->ui.pFpsFont)
    {
        printf("Freeing memory of: pFpsFont\n");
        TTF_CloseFont(pGame->ui.pFpsFont);
    }

    TTF_Quit();

    SDLNet_Quit();

    if (pGame->pRenderer)
    {
        printf("Freeing memory of: pRenderer\n");
        SDL_DestroyRenderer(pGame->pRenderer);
    }

    if (pGame->pWindow)
    {
        printf("Freeing memory of: pWindow\n");
        SDL_DestroyWindow(pGame->pWindow);
    }
#ifdef SDL_MIXER_H_
    Mix_FreeChunk(pGame->pCharge);
    Mix_FreeChunk(pGame->pHit);
    Mix_FreeChunk(pGame->pBonk);
    Mix_FreeChunk(pGame->pWin);
    Mix_FreeChunk(pGame->pMenuSwitch);

    Mix_FreeMusic(pGame->pMusic);

    Mix_CloseAudio();

    Mix_Quit();
#endif
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
    SDL_SetRenderDrawColor(pGame->pRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pGame->pRenderer);
    SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[19], NULL, &backGround);
    SDL_Rect temp;
    sem_wait(&pGame->pGameSemaphore);
    translatePositionToScreen(pGame);
    if (pGame->ui.playerTookDamage)
    {
        pGame->ui.playerTookDamage = false;
        pGame->ui.damageRedChannel = 255;
    }
    else if (pGame->ui.damageRedChannel > 0)
    {
        pGame->ui.damageRedChannel--;
    }
    
    pGame->isDrawing = true; // temporary fix to screen-tearing?
    int darkness = 0;
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (((pGame->map[i].wall.x <= pGame->windowWidth) && (pGame->map[i].wall.x + pGame->world.tileSize >= 0)) && ((pGame->map[i].wall.y <= pGame->windowHeight) && (pGame->map[i].wall.y + pGame->world.tileSize >= 0)))
        {
            if (pGame->pPlayer->state == ALIVE)
                darkness = (255 * ((float)(abs(pGame->map[i].x - pGame->pPlayer->x) + abs(pGame->map[i].y - pGame->pPlayer->y)) / (16 * pGame->world.tileSize)));
            if (darkness > 255)
                darkness = 255;
            SDL_SetRenderDrawBlendMode(pGame->pRenderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(pGame->pRenderer, pGame->ui.damageRedChannel, 0, 0, darkness);
            if (pGame->map[i].type > 0)
            {
                SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[pGame->map[i].type], NULL, &pGame->map[i].wall);
            }
            else
            {
                if (i > MAPSIZE - 1)
                {
                    if (pGame->map[i - MAPSIZE].type == 1)
                    {
                        temp = pGame->map[i].wall;
                        temp.h = ((float)pGame->world.tileSize * pGame->world.angle);

                        SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[(pGame->map[i - MAPSIZE].type)], NULL, &temp);

                        SDL_RenderFillRect(pGame->pRenderer, &temp);
                    }
                    else if (pGame->map[i - MAPSIZE].type > 0)
                    {
                        temp = pGame->map[i].wall;
                        temp.h = ((float)pGame->world.tileSize * pGame->world.angle);
                        SDL_RenderCopy(pGame->pRenderer, pGame->pTileTextures[2], NULL, &temp);
                        SDL_RenderFillRect(pGame->pRenderer, &temp);
                    }
                }
            }
            if (i == (((pGame->pPlayer->y / pGame->map[0].wall.w) * MAPSIZE) + ((pGame->pPlayer->x - 1) / pGame->map[0].wall.w) + 2))
            {
                drawPlayer(pGame, *pGame->pPlayer, pGame->pPlayer->id);
            }
            for (int j = 0; j < pGame->nrOfPlayers; j++)
            {
                if (i == (((pGame->pMultiPlayer[j].y / pGame->map[0].wall.w) * MAPSIZE) + ((pGame->pMultiPlayer[j].x - 1) / pGame->map[0].wall.w) + 2))
                {
                    drawPlayer(pGame, pGame->pMultiPlayer[j], pGame->pMultiPlayer[j].id);
                    if (pGame->pMultiPlayer[j].name[0])
                    {
                        pGame->ui.pPlayerName = createText(pGame->pRenderer, 0, 0, 0, pGame->ui.pNameTagFont, pGame->pMultiPlayer[j].name, pGame->pMultiPlayer[j].rect.x + (pGame->pMultiPlayer[j].rect.w / 2), pGame->pMultiPlayer[j].rect.y);
                        drawText(pGame->ui.pPlayerName, pGame->pRenderer);
                        freeText(pGame->ui.pPlayerName);
                    }
                }
            }

            SDL_SetRenderDrawColor(pGame->pRenderer, pGame->ui.damageRedChannel, 0, 0, darkness);

            if (pGame->map[i].type <= 0)
            {
                temp = pGame->map[i].wall;
                temp.y += ((float)pGame->world.tileSize * pGame->world.angle);
                SDL_RenderFillRect(pGame->pRenderer, &temp);
            }
            else
                SDL_RenderFillRect(pGame->pRenderer, &pGame->map[i].wall);
        }
    }
    pGame->isDrawing = false; // temporary fix to screen-tearing?

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
    sem_post(&pGame->pGameSemaphore);
    return NULL;
}
