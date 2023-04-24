#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "definitions.h"
#include "text.h"

#define MAX_PLAYERS 5
#define PORT 1234

enum serverState{JOINING, RUNNING, CLOSED};
typedef enum serverState ServerState;

typedef struct info {
    IPaddress address;
    int id;
} Info;

typedef struct server {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    TTF_Font *pFont;
    Text *pSpace, *pClosed, *pJoining, *pRunning;

    ServerState state;
    int windowWidth;
    int windowHeight;

    int nrOfClients;
    Info clients[MAX_PLAYERS];
    UDPsocket socketDesc;   // Socket descriptor
    UDPpacket *pRecieve;    // Pointer to packet memory
    UDPpacket *pSent;           
} Server;

int setup(Server *pServer);
void run(Server *pServer);
void close(Server *pServer);
void addClient(Server *pServer);
void recieveAndSend(Server *pServer);

int main(int argc, char **argv) {
    Server s;
    if (!setup(&s)) return 1;
    run(&s);
    close(&s);
}

int setup(Server *pServer) {
    // Basic inits
    if (SDL_Init(SDL_INIT_EVERYTHING)!=0) {
        printf("Error: %s\n",SDL_GetError());
        return 0;
    }
    if (TTF_Init()!=0) {
        printf("Error: %s\n",TTF_GetError());
        SDL_Quit();
        return 0;
    }
    if (SDLNet_Init()) {
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        TTF_Quit();
        SDL_Quit();
		return 0;
	}

    // Init window
    char windowName[30];
    sprintf(windowName, "Kittenfork Server");

    SDL_DisplayMode display;
    if (SDL_GetDesktopDisplayMode(0, &display) < 0) {
        printf("SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
        return 0;
    }

    pServer->windowWidth  = (float) display.w * 0.7; // 40% of avaliable space
    pServer->windowHeight = (float) display.h * 0.7;

    pServer->pWindow = SDL_CreateWindow(windowName,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,pServer->windowWidth,pServer->windowHeight,0);
    if (!pServer->pWindow) {
        printf("Error: %s\n",SDL_GetError());
        close(pServer);
        return 0;
    }
    pServer->pRenderer = SDL_CreateRenderer(pServer->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!pServer->pRenderer) {
        printf("Error: %s\n",SDL_GetError());
        close(pServer);
        return 0;    
    }

    // Init font
    pServer->pFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", 100);
    if (!pServer->pFont) {
        printf("Error: %s\n", TTF_GetError());
        close(pServer);
        return 1;
    }

    // Open socket
    if (!(pServer->socketDesc = SDLNet_UDP_Open(PORT))) {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    // Make space for future packets
    if (!((pServer->pSent = SDLNet_AllocPacket(512)) && (pServer->pRecieve = SDLNet_AllocPacket(512)))) {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE); 
    }

    pServer->pSpace   = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, "Press space", pServer->windowWidth / 2, pServer->windowHeight / 3);
    pServer->pJoining = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, "to start hosting", pServer->windowWidth / 2, pServer->windowHeight / 3 + 8 * TILESIZE);
    pServer->pRunning = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, "Server is running", pServer->windowWidth / 2, pServer->windowHeight / 2);
    pServer->pClosed  = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, "to open server", pServer->windowWidth / 2, pServer->windowHeight / 3 + 8 * TILESIZE);

    if (!pServer->pSpace || !pServer->pJoining || !pServer->pRunning || !pServer->pClosed) {
        printf("Error: %s\n", SDL_GetError());
        close(pServer);
    }

    pServer->state = CLOSED;
    pServer->nrOfClients = 0;
}

void run(Server *pServer) {
    int quit = 0, draw = 0; 
    SDL_Event event;
    char prompt[12];

    while (!quit) {
        SDL_SetRenderDrawColor(pServer->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pServer->pRenderer);

        sprintf(prompt, "Clients: %d", pServer->nrOfClients);
        Text *pPrompt = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, prompt, pServer->windowWidth / 2, pServer->windowHeight / 3 + 20 * TILESIZE);

        switch (pServer->state) {
        case CLOSED:
            drawText(pServer->pSpace, pServer->pRenderer);
            drawText(pServer->pClosed, pServer->pRenderer);
            if(SDL_PollEvent(&event)) { 
                if (event.type==SDL_QUIT) quit = 1; 
                if (event.type==SDL_KEYDOWN) 
                    if (event.key.keysym.sym == SDLK_SPACE) pServer->state = JOINING;
            }
            break;
        case JOINING:
            drawText(pServer->pSpace, pServer->pRenderer);
            drawText(pServer->pJoining, pServer->pRenderer);
            drawText(pPrompt, pServer->pRenderer);
            freeText(pPrompt);

            if(SDL_PollEvent(&event)) { 
                if (event.type==SDL_QUIT) quit = 1; 
                if (event.type==SDL_KEYDOWN && pServer->nrOfClients >= 2) 
                    if (event.key.keysym.sym == SDLK_SPACE) pServer->state = RUNNING;
            }
            if(SDLNet_UDP_Recv(pServer->socketDesc, pServer->pRecieve)==1 && pServer->nrOfClients < MAX_PLAYERS) addClient(pServer);
            break;
        case RUNNING:
            drawText(pServer->pRunning, pServer->pRenderer);
            drawText(pPrompt, pServer->pRenderer);
            freeText(pPrompt);

            if(SDL_PollEvent(&event)) if(event.type==SDL_QUIT) quit = 1;
            if(SDLNet_UDP_Recv(pServer->socketDesc, pServer->pRecieve)==1) recieveAndSend(pServer);
            break;
        }

        SDL_RenderPresent(pServer->pRenderer);
    }
}

void close(Server *pServer) {
    if(pServer->pRenderer) SDL_DestroyRenderer(pServer->pRenderer);
    if(pServer->pWindow) SDL_DestroyWindow(pServer->pWindow);

    if(pServer->pFont) TTF_CloseFont(pServer->pFont);

    if(pServer->pRecieve) SDLNet_FreePacket(pServer->pRecieve);
    if(pServer->pSent) SDLNet_FreePacket(pServer->pSent);
	if(pServer->socketDesc) SDLNet_UDP_Close(pServer->socketDesc);

    SDLNet_FreePacket(pServer->pSent);
    SDLNet_FreePacket(pServer->pRecieve);
    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}

/* Compares port to connected players ports and if there are less than 5 players adds new players to a list */
void addClient(Server *pServer) {
    for (int i = 0; i < MAX_PLAYERS; i++) if (pServer->pRecieve->address.port == pServer->clients[i].address.port) return;
    pServer->clients[pServer->nrOfClients].address = pServer->pRecieve->address;    // Adds client address
    pServer->clients[pServer->nrOfClients].id = pServer->nrOfClients + 1;           // Adds client id
    pServer->nrOfClients++;
}

/* Retrieves and resends data from UDP port. Rejects data from unknown players. */
void recieveAndSend(Server *pServer) {
    int id, known = 0;
    Player udpData;

    // Find packet sender among players
    for (int i = 0; i < pServer->nrOfClients; i++)
        if (pServer->pRecieve->address.port == pServer->clients[i].address.port) { id = pServer->clients[i].id; printf("Recieved data from player %d\n", id); known = 1; }

    // If client is not known, return
    if (!known) return;

    // Remove player if packet contains "exit" and adjust other players
    if (!strcmp((char *)pServer->pRecieve->data, "exit")) {
        printf("Player %d has exited\n", id);

        sprintf((char *)pServer->pSent->data, "%c exit", id+48); // 48 = 0 in ASCII 
        pServer->pSent->len = strlen((char *)pServer->pSent->data) + 1;
        SDLNet_UDP_Send(pServer->socketDesc, -1, pServer->pSent);

        for (int i = id-1; i < pServer->nrOfClients-1; i++) {
            pServer->clients[i] = pServer->clients[i+1];
            pServer->clients[i].id--;
            printf("- Player %d is now player %d\n", i+2, pServer->clients[i].id);
        }
        pServer->nrOfClients--;
    } else {    // Mirror packet to other players
        for (int i = 0; i < pServer->nrOfClients; i++){
            if (id != pServer->clients[i].id) {  
                printf("Send to client %d\n", pServer->clients[i].id);

                pServer->pSent->address.host = pServer->clients[i].address.host;     // Set destination host
                pServer->pSent->address.port = pServer->clients[i].address.port;     // Set destination port

                // Copy recieved data to another package and print for error handling
                memcpy(&udpData, (char *)pServer->pRecieve->data, sizeof(Player));
                memcpy((char *)pServer->pSent->data, &udpData, sizeof(Player)+1);

                //sprintf((char *)pSent->data, "%s", a);
                pServer->pSent->len = sizeof(Player) + 1;
                SDLNet_UDP_Send(pServer->socketDesc, -1, pServer->pSent);
            }
        }
    }
}