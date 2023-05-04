#include "serverUDP.h"
#include "TCPserver.h"
#include "definitions.h"
#include "text.h"

int universalSetup(Server *pServer);
void universalClose(Server *pServer);

int main(int argc, char **argv) {
    Server s;
    if(!universalSetup(&s)) return 1;
    if (!TCPserver(&s)) serverUDP(&s);
    universalClose(&s);
    return 0;
}

int universalSetup(Server *pServer) {
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

    pServer->windowWidth  = (float) display.w * 0.4; // 40% of avaliable space
    pServer->windowHeight = (float) display.h * 0.4;

    pServer->pWindow = SDL_CreateWindow(windowName,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,pServer->windowWidth,pServer->windowHeight,0);
    if (!pServer->pWindow) {
        printf("Error: %s\n",SDL_GetError());
        universalClose(pServer);
        return 0;
    }
    pServer->pRenderer = SDL_CreateRenderer(pServer->pWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!pServer->pRenderer) {
        printf("Error: %s\n",SDL_GetError());
        universalClose(pServer);
        return 0;    
    }

    pServer->fontSize = (float)pServer->windowWidth / 20;
    // Init font
    pServer->pFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pServer->fontSize);
    if (!pServer->pFont) {
        printf("Error: %s\n", TTF_GetError());
        universalClose(pServer);
        return 1;
    }

    pServer->pSpace   = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, "Press space",        pServer->windowWidth / 2, 2*pServer->fontSize);
    pServer->pJoining = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, "Hosted at",          pServer->windowWidth / 2, 2*pServer->fontSize);
    pServer->pRunning = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, "Server is running",  pServer->windowWidth / 2, 4*pServer->fontSize);
    pServer->pClosed  = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, "to open server",     pServer->windowWidth / 2, 4*pServer->fontSize);

    if (!pServer->pSpace || !pServer->pJoining || !pServer->pRunning || !pServer->pClosed) {
        printf("Error: %s\n", SDL_GetError());
        universalClose(pServer);
    }

    pServer->state = CLOSED;
    pServer->nrOfClients = 0;
}

void universalClose(Server *pServer) {
    if(pServer->pRenderer) SDL_DestroyRenderer(pServer->pRenderer);
    if(pServer->pWindow) SDL_DestroyWindow(pServer->pWindow);
    if(pServer->pFont) TTF_CloseFont(pServer->pFont);

    SDLNet_Quit();
    TTF_Quit();    
    SDL_Quit();
}