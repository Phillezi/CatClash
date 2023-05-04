#include "multiThreadedServer.h"

void *MThostServer(void *mapName)
{
    Threads threads;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_cleanup_push(closeThreads, (void *)&threads);

    bool exit = false;
    initMap(threads.server.map, (char *)mapName, 16);
    int prevTime = 0;
    if (!MTsetup(&threads.server))
    {
        while (!exit)
        {
            if (pthread_create(&threads.tcp, NULL, MTtcpServer, (void *)&threads.server))
                printf("Could not create Thread TCP\n");
            if (pthread_create(&threads.udp, NULL, MTudpServer, (void *)&threads.server))
                printf("Could not create Thread UDP\n");

            int deltaTime = SDL_GetTicks() - prevTime;
            if (deltaTime >= 1000 / 30)
            {
                prevTime = SDL_GetTicks();
                MTupdateServerScreen(&threads.server);
            }
            pthread_testcancel();

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    printf("Server: SDL QUIT EVENT\n");
                    exit = true;
                    break;
                }
                else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    printf("Server: SDL window close event\n");

                    if (SDL_GetWindowID(threads.server.pWindow) == event.window.windowID)
                    {
                        printf("Server: For This Window\n");
                        exit = true;
                        break;
                    }
                    else
                    {
                        debugPrint();
                    }
                }
            }
            pthread_join(threads.tcp, NULL);
            pthread_join(threads.udp, NULL);
        }
    }
    closeThreads((void *)&threads);
    pthread_cleanup_pop(0);
    pthread_exit(NULL);
}

void closeThreads(void *pThreadsIn)
{
    Threads *pT = (Threads *)pThreadsIn;

    printf("Cleaning up Threads...\n");
    if (pT->tcp)
    {
        pthread_cancel(pT->tcp);
        printf("Cancelling TCP...\n");
        pthread_join(pT->tcp, NULL);
        printf("Done: Cancelling TCP!\n");
    }
    if (pT->udp)
    {
        pthread_cancel(pT->udp);
        printf("Cancelling UDP...\n");
        pthread_join(pT->udp, NULL);
        printf("Done: Cancelling UDP!\n");
    }
    printf("Done: Cleaning up Threads!\n");
    printf("Cleaning up server...\n");
    MTclose(&pT->server);
    printf("Done: Cleaning up server!\n");
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

    pServer->nrOfClients = 0;

    pServer->pWindow = SDL_CreateWindow("TCP & UDP SERVER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pServer->windowWidth, pServer->windowHeight, 0);
    if (!pServer->pWindow)
    {
        debugPrint();
        return 1;
    }

    pServer->pRenderer = SDL_CreateRenderer(pServer->pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pServer->pRenderer)
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
    if (!pServer->pFont)
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
    TCPsocket tmpClient;
    int running = 1, bytesSent = 0, bytesRecv = 0;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    pthread_cleanup_push(NULL, NULL);

    pthread_testcancel();

    tmpClient = SDLNet_TCP_Accept(pServer->socketTCP);
    if (tmpClient)
    {
        if (pServer->nrOfClients < MAX_PLAYERS)
        {
            SDLNet_TCP_AddSocket(pServer->socketSetTCP, tmpClient);
            printf("User: %d joined!\n", ++pServer->nrOfClients);
            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                bytesSent = SDLNet_TCP_Send(tmpClient, &pServer->map[i].type, sizeof(pServer->map[i].type)); // send map to client
                if (bytesSent != sizeof(pServer->map[i].type))
                {
                    printf("Error: packet loss when sending map nr:%d\n", i);
                }
            }
            printf("Done: Sending map\n");
            /*
            Uint8 clientRecvCorrectlyFlag = 0;
            bytesRecv = SDLNet_TCP_Recv(tmpClient, &clientRecvCorrectlyFlag, sizeof(Uint8));
            if(bytesRecv != sizeof(Uint8)){
                printf("Error: packet loss when reciving clientRecvCorrectlyFlag\n");
            }
            */

            bytesSent = SDLNet_TCP_Send(tmpClient, &pServer->nrOfClients, sizeof(pServer->nrOfClients));
            if (bytesSent != sizeof(pServer->nrOfClients))
            {
                printf("Error: packet loss when sending player id\n");
            }

            bytesRecv = SDLNet_TCP_Recv(tmpClient, &pServer->clients[pServer->nrOfClients].data, sizeof(Player));
            if (bytesRecv != sizeof(Player))
            {
                printf("Error when reciving Player struct over TCP\n");
                debugPrint();
            }
            printf("Player %d joined\n", pServer->clients[pServer->nrOfClients].data.id);
            for (int i = 0; i < pServer->nrOfClients - 1; i++)
            {
                SDLNet_TCP_Send(tmpClient, &pServer->clients[i].data, sizeof(Player));
                printf("Sent Playerdata of id: %d to new player\n", pServer->clients[i].data.id);
                SDLNet_TCP_Send(pServer->clients[i].tcpSocket, &pServer->clients[pServer->nrOfClients].data, sizeof(Player));
            }
            printf("DONE: reciving and sending PLAYERDATA!\n");
            SDLNet_TCP_Close(tmpClient); // close client
            printf("I didnt crash here\n");
            char buffer[31];
            sprintf(buffer, "%d Players Connected", pServer->nrOfClients);
            if (pServer->pJoining)
                freeText(pServer->pJoining);
            pServer->pJoining = createText(pServer->pRenderer, 255, 255, 255, pServer->pFont, buffer, pServer->windowWidth / 2, 2 * pServer->fontSize);
        }
        else
        {
            printf("MAX players reached\n");
        }
    }
    /*
    while (SDLNet_CheckSockets(pServer->socketSetTCP, 0) > 0)
    {
        for (int i = 0; i < pServer->nrOfClients; i++)
        {
            if (SDLNet_SocketReady(pServer->clients[i].tcpSocket))
            {
                pServer->clients[i].timeout = SDL_GetTicks();
            }
        }
    }
    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        if (SDL_GetTicks() - pServer->clients[i].timeout > 5000)
        {
            // PLAYER TIMED OUT
        }
    }
    */

    pthread_cleanup_pop(0);
    pthread_exit(NULL);
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

    if (!((pServer->pSent = SDLNet_AllocPacket(sizeof(PlayerUdpPkg))) && (pServer->pRecieve = SDLNet_AllocPacket(sizeof(PlayerUdpPkg)))))
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

void MTcheckUdpClient(Server *pServer, PlayerUdpPkg data)
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
                sprintf(buffer, "%d %s", data.id, pServer->clients[data.id].data.name);
                pServer->pClientText[i] = createText(pServer->pRenderer, 255, 255, 255, pServer->pFont, buffer, pServer->windowWidth / 2, i * pServer->fontSize + 3 * pServer->fontSize);
                return;
            }
        }
    }
}

void *MTudpServer(void *pServerIn)
{
    Server *pServer = (Server *)pServerIn;
    int running = 1;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    pthread_cleanup_push(NULL, NULL);

    pthread_testcancel();

    /*
    Kolla om alla i clients har skickat ett meddelande inom ett visst intervall (keep alive signaler ksk behöver skickas från klientsidan)
    Om servern inte har tagit emot ett meddelande från en klient så har den timeat ut och då bör den platsen i clientArrayen sättas till tom
    dvs port och host bör sättas till 8888
    */
    if (SDLNet_UDP_Recv(pServer->socketUDP, pServer->pRecieve) == 1)
    {
        PlayerUdpPkg data;
        /*
        Ta emot mindre(i bytes) structar som innehåller bara x y riktning och id
        */
        memcpy(&data, pServer->pRecieve->data, sizeof(PlayerUdpPkg));
        MTcheckUdpClient(pServer, data);
        for (int i = 0; i < pServer->nrOfClients; i++)
        {
            if (pServer->clients[i].id != data.id && pServer->clients[i].address.port != 8888)
            {
                /*
                Skicka mindre(i bytes) structar som innehåller bara x y riktning och id
                */
                memcpy(pServer->pSent->data, &data, sizeof(PlayerUdpPkg));
                pServer->pSent->address.port = pServer->clients[i].address.port;
                pServer->pSent->address.host = pServer->clients[i].address.host;
                pServer->pSent->len = sizeof(PlayerUdpPkg);

                if (!SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pSent))
                {
                    printf("Error: Could not send package\n");
                }
            }
        }
    }

    pthread_cleanup_pop(0);
    pthread_exit(NULL);
}

void MTupdateServerScreen(Server *pServer)
{
    SDL_SetRenderDrawColor(pServer->pRenderer, 20, 20, 20, 255);
    SDL_RenderClear(pServer->pRenderer);

    if (pServer->pJoining)
        drawText(pServer->pJoining, pServer->pRenderer);
    for (int i = 0; i < MAX_PLAYERS; i++)
        if (pServer->pClientText[i])
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

    // CLOSE TTF
    for (int i = 0; i < MAX_PLAYERS; i++)
        if (pServer->pClientText[i])
            freeText(pServer->pClientText[i]);

    if (pServer->pJoining)
        freeText(pServer->pJoining);

    if (pServer->pIP)
        freeText(pServer->pIP);

    if (pServer->pFont)
        TTF_CloseFont(pServer->pFont);
}