#include "multiThreadedServer.h"

void *MThostServer(void *map)
{
    Server server;
    pthread_t tcpThread;
    pthread_t udpThread;
    bool exit = false;
    memcpy(server.map, (Tile *)map, sizeof((Tile *)map));
    if (!MTsetup(&server))
    {
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
            }
            pthread_create(&tcpThread, NULL, MTtcpServer, (void *)&server);
            pthread_create(&udpThread, NULL, MTudpServer, (void *)&server);
            MTupdateServerScreen(&server); // VSYNC är på dvs kommer den vara blockerande
            pthread_join(tcpThread, NULL);
            pthread_join(udpThread, NULL);
        }
    }
    MTclose(&server);
}

void debugPrint()
{
    printf("Error: %s\n", SDL_GetError());
}

int MTsetup(Server *pServer)
{
    // INITIALIZE SDL
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        debugPrint();
        return 1;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
    {
        debugPrint();
        return 1;
    }

    pServer->windowWidth = (float)displayMode.w * 0.4; // 40% of avaliable space
    pServer->windowHeight = (float)displayMode.h * 0.4;

    pServer->fontSize = pServer->windowHeight / 10;

    pServer->pWindow = SDL_CreateWindow("TCP & UDP SERVER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pServer->windowWidth, pServer->windowHeight, 0);
    if(!pServer->pWindow)
    {
        debugPrint();
        return 1;
    }

    pServer->pRenderer = SDL_CreateRenderer(pServer->pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!pServer->pRenderer)
    {
        debugPrint();
        return 1;
    }

    // INITIALIZE TTF
    if (TTF_Init())
    {
        debugPrint();
        return 1;
    }

    pServer->pFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pServer->fontSize);
    if(!pServer->pFont)
    {
        debugPrint();
        return 1;
    }

    // INITIALIZE SDLNet
    if (SDLNet_Init())
    {
        debugPrint();
        return 1;
    }

    if (MTsetupTCP(pServer))
    {
        debugPrint();
        return 1;
    }

    if (MTsetupUDP(pServer))
    {
        debugPrint();
        return 1;
    }

    return 0;
}

int MTsetupTCP(Server *pServer)
{
    if (SDLNet_ResolveHost(&pServer->TCPip, NULL, PORT))
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return 1;
    }
    pServer->socketSetTCP = SDLNet_AllocSocketSet(MAX_PLAYERS);
    if (!pServer->socketSetTCP)
    {
        fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return 1;
    }
    pServer->socketTCP = SDLNet_TCP_Open(&pServer->TCPip);
    if (!pServer->socketTCP)
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        return 1;
    }

    return 0;
}

void *MTtcpServer(void *pServerIn)
{
    Server *pServer = (Server *)pServerIn;

    TCPsocket tmpClient = SDLNet_TCP_Accept(pServer->socketTCP);
    if (tmpClient)
    {
        if (pServer->nrOfClients < MAX_PLAYERS)
        {
            SDLNet_TCP_AddSocket(pServer->socketSetTCP, tmpClient);
            printf("User: %d joined!\n", ++pServer->nrOfClients);
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                SDLNet_TCP_Send(tmpClient, &pServer->map[i].type, sizeof(pServer->map[i].type)); // send map to client
            SDLNet_TCP_Send(tmpClient, &pServer->nrOfClients, sizeof(int));
            SDLNet_TCP_Close(tmpClient); // close client
            char buffer[31];
            sprintf(buffer, "%d Players Connected", pServer->nrOfClients);
            pServer->pJoining = createText(pServer->pRenderer, 255, 255, 255, pServer->pFont, buffer, pServer->windowWidth/2, 2*pServer->fontSize);
        }
        else
        {
            printf("MAX players reached\n");
        }
    }
    /*
    for(pServer->nrOfClients; pServer->nrOfClients > 0; pServer->nrOfClients--)
    {
        SDLNet_TCP_Close(tmpClient);
    }
    */
}

int MTsetupUDP(Server *pServer)
{
    if (!(pServer->socketUDP = SDLNet_UDP_Open(PORT)))
    {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }

    if (!((pServer->pSent = SDLNet_AllocPacket(sizeof(Player))) && (pServer->pRecieve = SDLNet_AllocPacket(512))))
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }

    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        pServer->clients[i].address.port = 8888;
        pServer->clients[i].address.host = 8888;
        pServer->clients[i].id = 0;
    }

    return 0;
}

void MTcheckUdpClient(Server *pServer, Player data)
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (pServer->clients[i].address.port == pServer->pRecieve->address.port && pServer->clients[i].address.host == pServer->pRecieve->address.host) // is this the sender?
        {
            // sender is known
            return;
        }
        else // no
        {
            if ((pServer->clients[i].address.port == 8888) && (pServer->clients[i].address.host == 8888)) // is it a empty slot?
            {
                pServer->clients[i].address.port = pServer->pRecieve->address.port; // Save client in slot
                pServer->clients[i].address.host = pServer->pRecieve->address.host;
                pServer->clients[i].id = data.id;
                char buffer[32];
                sprintf(buffer, "%d %s", data.id, data.name);
                pServer->pClientText[i] = createText(pServer->pRenderer, 255, 255, 255, pServer->pFont, buffer, pServer->windowWidth/2, i*pServer->fontSize + 3*pServer->fontSize);
                return;
            }
        }
    }
}

void *MTudpServer(void *pServerIn)
{
    Server *pServer = (Server *)pServerIn;
    if (SDLNet_UDP_Recv(pServer->socketUDP, pServer->pRecieve) == 1)
    {
        Player data;
        memcpy(&data, pServer->pRecieve->data, sizeof(Player));
        MTcheckUdpClient(pServer, data);
        for (int i = 0; i < pServer->nrOfClients; i++)
        {
            if (pServer->clients[i].id != data.id && pServer->clients[i].address.port != 8888)
            {
                memcpy(pServer->pSent->data, &data, sizeof(Player));
                pServer->pSent->address.port = pServer->clients[i].address.port;
                pServer->pSent->address.host = pServer->clients[i].address.host;
                pServer->pSent->len = sizeof(Player);

                if (!SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pSent))
                {
                    printf("Error: Could not send package\n");
                }
            }
        }

    }
}

void MTupdateServerScreen(Server *pServer){
    SDL_SetRenderDrawColor(pServer->pRenderer, 20, 20, 20, 255);
    SDL_RenderClear(pServer->pRenderer);

    if(pServer->pJoining)
        drawText(pServer->pJoining, pServer->pRenderer);
    for(int i = 0; i < MAX_PLAYERS; i++)
        if(pServer->pClientText[i])
            drawText(pServer->pClientText[i], pServer->pRenderer);

    SDL_RenderPresent(pServer->pRenderer);
}

void MTclose(Server *pServer)
{
    // CLOSE NET
    if (pServer->socketSetTCP)
        SDLNet_FreeSocketSet(pServer->socketSetTCP);
    if (pServer->socketTCP)
        SDLNet_TCP_Close(pServer->socketTCP);

    if (pServer->pRecieve)
        SDLNet_FreePacket(pServer->pRecieve);

    if (pServer->pSent)
        SDLNet_FreePacket(pServer->pSent);

    if (pServer->socketUDP)
        SDLNet_UDP_Close(pServer->socketUDP);

    SDLNet_Quit();

    // CLOSE TTF
    for(int i = 0; i < MAX_PLAYERS; i++)
        if(pServer->pClientText[i])
            freeText(pServer->pClientText[i]);

    if(pServer->pJoining)
        freeText(pServer->pJoining);
    
    if(pServer->pIP)
        freeText(pServer->pIP);

    if(pServer->pFont)
        TTF_CloseFont(pServer->pFont);

    TTF_Quit();

    // CLOSE SDL
    if(pServer->pRenderer)
        SDL_DestroyRenderer(pServer->pRenderer);

    if(pServer->pWindow)
        SDL_DestroyWindow(pServer->pWindow);

    SDL_Quit();
}