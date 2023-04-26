#include "definitions.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>
#include "init.h"
#include "text.h"

#define MAX_SOCKETS 5
#define PORT 1234

int setupTCP(Server *pServer);
int runTCP(Server *pServer, Tile map[], char mapName[]);
void closeTCP(Server *pServer);
int mmapSelection(Server *pServer, Tile map[]);
int getStringFromUser(char text[], SDL_Event event);

int TCPserver(Server *pServer)
{
    Tile map[1024];
    char mapName[100];
    int quit = 0;

    if (!setupTCP(pServer)) return 1;
    if (runTCP(pServer, map, mapName)) quit = 1;
    closeTCP(pServer);
    return quit;
}

int setupTCP(Server *pServer) {
    if (SDLNet_ResolveHost(&pServer->TCPip, NULL, PORT)) {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    pServer->socketSetTCP = SDLNet_AllocSocketSet(MAX_SOCKETS);
    if (!pServer->socketSetTCP) {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    pServer->socketTCP = SDLNet_TCP_Open(&pServer->TCPip);
    if (!pServer->socketTCP) {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    char ip[30];
    sprintf(ip, "%s:%d", SDLNet_ResolveIP(&pServer->TCPip), PORT);

    pServer->pIP = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, ip, pServer->windowWidth / 2, 4*(pServer->windowHeight / MAPSIZE) * 4);

    if (!pServer->pIP) {
        printf("Error: %s\n", SDL_GetError());
        closeTCP(pServer);
    }

    // char text[31] = {0};
    // do{
    //     printf("Which map do you want to host?\n:");
    //     scanf(" %100s", mapName);
    // } while(initMap(map, mapName, 16));

    return 1;
}

int runTCP(Server *pServer, Tile map[], char mapName[]) {
    int quit = 0, tileSize = 0; 
    SDL_Event event;
    Player udpData;
    char prompt[12];

    while (!quit) {
        SDL_SetRenderDrawColor(pServer->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pServer->pRenderer);

        sprintf(prompt, "Clients: %d", pServer->nrOfClients);
        Text *pPrompt = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, prompt, pServer->windowWidth / 2, 6*(pServer->windowHeight / MAPSIZE) * 4);

        switch (pServer->state) 
        {
        case CLOSED: 
            if(!mmapSelection(pServer, map)) pServer->state = JOINING;
            else return 1; 
            break;
        case JOINING:
                drawText(pServer->pJoining, pServer->pRenderer);
                drawText(pServer->pIP, pServer->pRenderer);
                drawText(pPrompt, pServer->pRenderer);

                TCPsocket tmpClient = SDLNet_TCP_Accept(pServer->socketTCP);
                if (tmpClient) {
                    if (pServer->nrOfClients < MAX_SOCKETS) {
                        SDLNet_TCP_AddSocket(pServer->socketSetTCP, tmpClient);
                        printf("User: %d joined!\n", ++pServer->nrOfClients);
                        for(int i = 0; i < MAPSIZE*MAPSIZE; i++)
                            SDLNet_TCP_Send(tmpClient, &map[i].type, sizeof(map[i].type));  // send map to client
                        SDLNet_TCP_Close(tmpClient);                                        // close client
                    }
                }
                
                if(SDL_PollEvent(&event)) { 
                if (event.type==SDL_QUIT) return 1; 
                if (event.type==SDL_KEYDOWN && pServer->nrOfClients >= 2) 
                    if (event.key.keysym.sym == SDLK_SPACE) { pServer->state = RUNNING; quit = 1; }
            }
            break;
        }

        freeText(pPrompt);
        SDL_RenderPresent(pServer->pRenderer);
    }
    
}

void closeTCP(Server *pServer) {
    SDLNet_FreeSocketSet(pServer->socketSetTCP);
    SDLNet_TCP_Close(pServer->socketTCP);
}

int mmapSelection(Server *pServer, Tile map[]) {
    int previousTime = 0;
    int quit = 0;
    Text *pPrompt = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, "Enter a map",pServer->windowWidth / 2, 1*(pServer->windowHeight / MAPSIZE) * 4);
    Text *pPrompt2 = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, "to host:", pServer->windowWidth / 2, 3*(pServer->windowHeight / MAPSIZE) * 4);
    Text *pMap = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, ":",               pServer->windowWidth / 2,  pServer->windowHeight / 2);

    char text[31] = {0};
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
                freeText(pMap);
                freeText(pPrompt);
                freeText(pPrompt2);
                return 1;
            }
            else {
                if (getStringFromUser(text, event)) {
                    if (initMap(map, text, TILESIZE)) {
                        printf("No file found\n");
                    } 
                    else return 0;
                } else {
                    if (text[0]) {
                        freeText(pMap);
                        pMap = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, text, pServer->windowWidth / 2, pServer->windowHeight / 2);
                    }
                }
            }
        }
        if (SDL_GetTicks() - previousTime >= 1000 / 60) {
            previousTime = SDL_GetTicks();

            SDL_SetRenderDrawColor(pServer->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pServer->pRenderer);

            drawText(pPrompt, pServer->pRenderer);
            drawText(pPrompt2, pServer->pRenderer);

            if (text[0]) {
                drawText(pMap, pServer->pRenderer);
            }

            SDL_RenderPresent(pServer->pRenderer);
        }
    }
    freeText(pMap);
    freeText(pPrompt);
    freeText(pPrompt2);
    return 0;
}

int getStringFromUser(char text[], SDL_Event event) {
    int strCounter = strlen(text), strEnd = 0;
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN) {
            if (strCounter) strEnd = 1;
        }
        if (event.key.keysym.sym == SDLK_BACKSPACE) {
            if (strCounter > 0) {
                text[strCounter - 1] = 0;
                strCounter--;
            }
        } else {
            if ((event.key.keysym.sym >= 'a' && event.key.keysym.sym <= '~') || (event.key.keysym.sym >= ' ' && event.key.keysym.sym <= '9')) {

                if (strCounter < 31) {
                    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
                    if (((event.key.keysym.sym > 96 && event.key.keysym.sym < 127)) && (currentKeyStates[SDL_SCANCODE_LSHIFT] || currentKeyStates[SDL_SCANCODE_RSHIFT]))
                        text[strCounter] = event.key.keysym.sym - 32;
                    else if (event.key.keysym.sym == '.' && (currentKeyStates[SDL_SCANCODE_LSHIFT] || currentKeyStates[SDL_SCANCODE_RSHIFT]))
                        text[strCounter] = ':';
                    else
                        text[strCounter] = event.key.keysym.sym;

                    strCounter++;
                }
            }
        }
    }
    return strEnd;
}
