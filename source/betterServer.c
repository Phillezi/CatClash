#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include "../include/definitions.h"
#include "../include/init.h"
#include "../include/text.h"
#include "../include/ioHandler.h"
#include "../include/player.h"

#define PORT 1234

int initServer(Server *pServer);
int setupSDL(Server *pServer);
int setupText(Server *pServer);
int setupText(Server *pServer);
int tcpSetup(Server *pServer);
int udpSetup(Server *pServer);
int getMapToHost(Server *pServer);
void *sendMapToPlayer(void *pServerIn);

void checkIncommingTCP(Server *pServer);
void checkTCPForNewConnections(Server *pServer);
void addTCPClient(Server *pServer, TCPsocket client);
void checkTCPTimeout(Server *pServer);
void occupySpawntile(Server *pServer);
int getFreeClientId(Server *pServer);
void sendPlayerId(Server *pServer);
void getPlayerData(Server *pServer, int clientIndex);
void sendPlayerData(Server *pServer);
void updateTCPStateText(Server *pServer);

void checkIncommingUDP(Server *pServer);
void checkUDPClient(Server *pServer, PlayerUdpPkg data);

void updateServerWindow(Server *pServer);
void dbgPrint();
void closeS(Server *pServer);

int main(int argv, char **args)
{
    Server server;
    bool exit = false;
    server.tcpState = IDLE;
    if (!initServer(&server))
    {
        while (!exit)
        {
            checkIncommingTCP(&server);
            checkIncommingUDP(&server);
            updateServerWindow(&server);
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    exit = true;
                }
            }
        }
    }

    closeS(&server);
    return 1;
}

int setupSDL(Server *pServer)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        dbgPrint();
        return 1;
    }

    if (SDLNet_Init())
    {
        dbgPrint();
        return 1;
    }

    if (TTF_Init())
    {
        dbgPrint();
        return 1;
    }

    SDL_DisplayMode displayMode;
    if (SDL_GetDesktopDisplayMode(0, &displayMode) < 0)
    {
        dbgPrint();
        return 1;
    }

    pServer->windowWidth = (float)displayMode.w * 0.4; // 40% of avaliable space
    pServer->windowHeight = (float)displayMode.h * 0.4;

    pServer->fontSize = pServer->windowHeight / 10;

    pServer->nrOfClients = 0;

    pServer->pWindow = SDL_CreateWindow("TCP & UDP SERVER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pServer->windowWidth, pServer->windowHeight, 0);
    if (!pServer->pWindow)
    {
        dbgPrint();
        return 1;
    }

    pServer->pRenderer = SDL_CreateRenderer(pServer->pWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!pServer->pRenderer)
    {
        dbgPrint();
        return 1;
    }
    return 0;
}

int setupText(Server *pServer)
{
    if (TTF_Init())
    {
        dbgPrint();
        return 1;
    }

    pServer->pFont = TTF_OpenFont("resources/fonts/RetroGaming.ttf", pServer->fontSize);
    if (!pServer->pFont)
    {
        dbgPrint();
        return 1;
    }

    if (SDLNet_Init())
    {
        dbgPrint();
        return 1;
    }

    pServer->pServerStateText = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, "Server is Ready", pServer->windowWidth / 2, pServer->fontSize);
    pServer->progressBar.x = (pServer->windowWidth / 2) - ((pServer->fontSize * IDLE) / 2);
    pServer->progressBar.y = 1.5 * pServer->fontSize;
    pServer->progressBar.h = pServer->fontSize;
    pServer->progressBar.w = 0;

    pServer->updateScreenFlag = 1;

    return 0;
}

int tcpSetup(Server *pServer)
{
    pServer->nrOfClients = 0;

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

int udpSetup(Server *pServer)
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
        pServer->clients[i].id = 8888;
    }
    return 0;
}

int initServer(Server *pServer)
{
    // INITIALIZE SDL
    if (setupSDL(pServer))
        return 1;

    // INITIALIZE TTF
    if (setupText(pServer))
        return 1;

    // SETUP TCP
    if (tcpSetup(pServer))
        return 1;

    // SETUP UDP
    if (udpSetup(pServer))
        return 1;

    // SETUP MAP
    if (getMapToHost(pServer))
        return 1;

    return 0;
}

int getMapToHost(Server *pServer)
{
    int nrOfMaps = 0, selected = 0, previousTime = 0;
    char **mapfiles = checkFolderAndReturnList(SAVE_MAP_PATH, &nrOfMaps);
    bool exit = false;
    Text *pText[nrOfMaps];

    for (int i = 0; i < nrOfMaps; i++)
    {
        if (!(i == selected))
            pText[i] = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, mapfiles[i], pServer->windowWidth / 2, (pServer->windowHeight / 2) + ((i - selected) * pServer->fontSize));
        else
        {
            pText[i] = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, mapfiles[i], pServer->windowWidth / 2, (pServer->windowHeight / 2) + ((i - selected) * pServer->fontSize));
        }
    }

    while (!exit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                exit = true;
                for (int i = 0; i < nrOfMaps; i++)
                    freeText(pText[i]);
                freeTextList(mapfiles, nrOfMaps);
                return 1;
                break;
            }

            else if (event.type == SDL_MOUSEWHEEL || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.wheel.y < 0 && event.type == SDL_MOUSEWHEEL || event.key.keysym.sym == SDLK_DOWN) // scroll up
                {
                    if (selected < nrOfMaps - 1)
                    {
                        selected++;
                        for (int i = 0; i < nrOfMaps; i++)
                        {
                            freeText(pText[i]);
                            if (!(i == selected))
                                pText[i] = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, mapfiles[i], pServer->windowWidth / 2, (pServer->windowHeight / 2) + ((i - selected) * pServer->fontSize));
                            else
                            {
                                pText[i] = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, mapfiles[i], pServer->windowWidth / 2, (pServer->windowHeight / 2) + ((i - selected) * pServer->fontSize));
                            }
                        }
                        initMap(pServer->map, mapfiles[selected], pServer->fontSize / 10);
                    }
                }
                else if (event.wheel.y > 0 && event.type == SDL_MOUSEWHEEL || event.key.keysym.sym == SDLK_UP) // scroll down
                {
                    if (selected > 0)
                    {
                        selected--;
                        for (int i = 0; i < nrOfMaps; i++)
                        {
                            freeText(pText[i]);
                            if (!(i == selected))
                                pText[i] = createText(pServer->pRenderer, 100, 100, 100, pServer->pFont, mapfiles[i], pServer->windowWidth / 2, (pServer->windowHeight / 2) + ((i - selected) * pServer->fontSize));
                            else
                            {
                                pText[i] = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, mapfiles[i], pServer->windowWidth / 2, (pServer->windowHeight / 2) + ((i - selected) * pServer->fontSize));
                            }
                        }
                        initMap(pServer->map, mapfiles[selected], pServer->fontSize / 10);
                    }
                }
                else if (event.key.keysym.sym == SDLK_RETURN || event.button.button == SDL_BUTTON_LEFT)
                {
                    if (initMap(pServer->map, mapfiles[selected], 16))
                        return 1;
                    exit = true;
                }
            }
        }
        if (SDL_GetTicks() - previousTime >= 1000 / 60)
        {
            previousTime = SDL_GetTicks();

            SDL_SetRenderDrawColor(pServer->pRenderer, 255, 255, 255, 255);
            SDL_RenderClear(pServer->pRenderer);
            /*for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                if (pServer->map[i].type > 0 && pServer->map[i].type < TILES)
                    SDL_RenderCopy(pServer->pRenderer, pServer->pTileTextures[pServer->map[i].type], NULL, &pServer->map[i].wall);
            }*/
            for (int i = 0; i < nrOfMaps; i++)
            {
                if (i != selected)
                    drawText(pText[i], pServer->pRenderer);
            }
            drawText(pText[selected], pServer->pRenderer);

            SDL_RenderPresent(pServer->pRenderer);
        }
    }

    return 0;
}

void *sendMapToPlayer(void *pServerIn)
{
    Server *pServer = (Server *)pServerIn;
    int bytesSent = 0;
    for (pServer->mapPos = 0; pServer->mapPos < MAPSIZE * MAPSIZE; pServer->mapPos++)
    {
        bytesSent = SDLNet_TCP_Send(pServer->clients[pServer->nrOfClients].tcpSocket, &pServer->map[pServer->mapPos].type, sizeof(pServer->map[pServer->mapPos].type)); // send map to client
        if (bytesSent != sizeof(pServer->map[pServer->mapPos].type))
        {
            printf("Error: packet loss when sending map nr:%d\n", pServer->mapPos);
        }
    }
}

void checkTCPForNewConnections(Server *pServer)
{
    TCPsocket tmpClient = SDLNet_TCP_Accept(pServer->socketTCP);
    if (tmpClient)
    {
        printf("TCP ACCEPTED\n");
        Uint8 packet = 0;
        int bytesRecv = SDLNet_TCP_Recv(tmpClient, &packet, sizeof(packet));
        if (pServer->nrOfClients < MAX_PLAYERS && bytesRecv == sizeof(packet) && packet)
        {
            addTCPClient(pServer, tmpClient);
        }
        else
            printf("Connection was just ping\n");
    }
}

void addTCPClient(Server *pServer, TCPsocket client)
{
    pServer->clients[pServer->nrOfClients].tcpSocket = client;
    SDLNet_TCP_AddSocket(pServer->socketSetTCP, pServer->clients[pServer->nrOfClients].tcpSocket);
    printf("User: %d joined!\n", pServer->nrOfClients);
    pServer->tcpState = CLIENT_JOINING;
}

void checkTCPTimeout(Server *pServer)
{
    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        if (SDL_GetTicks() - pServer->clients[i].timeout > 5000)
        {
            printf("Player %d timed out\n Disconnection them\n", i);
            pServer->clients[i].data.disconnectedFlag = 1;
            for (int j = 0; j < pServer->nrOfClients; j++)
            {
                if (j != i)
                {
                    SDLNet_TCP_Send(pServer->clients[j].tcpSocket, &pServer->clients[i].data, sizeof(Player));
                    printf("Sent disconnect info to player %d about %d\n", j, i);
                }
            }
            SDLNet_TCP_Close(pServer->clients[i].tcpSocket);
            SDLNet_DelSocket(pServer->socketSetTCP, (SDLNet_GenericSocket)pServer->clients[i].tcpSocket);
            for (i; i < pServer->nrOfClients; i++)
            {
                if (i + 1 < MAX_PLAYERS)
                {
                    pServer->clients[i] = pServer->clients[i + 1];
                    pServer->pClientText[i] = pServer->pClientText[i + 1];
                }
                else
                {
                    pServer->clients[i].address.port = 8888;
                    pServer->clients[i].address.host = 8888;
                    pServer->clients[i].id = 8888;
                    freeText(pServer->pClientText[i]);
                }
            }
            pServer->nrOfClients--;
            pServer->updateScreenFlag = 1;
        }
    }
}

void occupySpawntile(Server *pServer)
{
    for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
    {
        if (pServer->map[i].type == -1)
        {
            pServer->map[i].type = 0;
            i = MAPSIZE * MAPSIZE;
        }
    }
}

int getFreeClientId(Server *pServer)
{
    int id = 0;
    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        if (id == pServer->clients[i].id)
        {
            id++;  // increment id
            i = 0; // restart loop to check if id already exists
        }
    }
    return id;
}

void sendPlayerId(Server *pServer)
{
    int id = getFreeClientId(pServer);
    int bytesSent = SDLNet_TCP_Send(pServer->clients[pServer->nrOfClients].tcpSocket, &id, sizeof(id));
    if (bytesSent != sizeof(id))
    {
        printf("Error: packet loss when sending player id\n");
    }
    pServer->tcpState++;
}

void getPlayerData(Server *pServer, int clientIndex)
{
    pServer->clients[clientIndex].timeout = SDL_GetTicks();
    int bytesRecv = SDLNet_TCP_Recv(pServer->clients[pServer->nrOfClients].tcpSocket, &pServer->clients[pServer->nrOfClients].data, sizeof(Player));
    if (bytesRecv != sizeof(Player))
    {
        printf("Error when reciving Player struct over TCP\n");
        dbgPrint();
    }
    char buffer[32];
    sprintf(buffer, "%d %s", pServer->clients[pServer->nrOfClients].data.id, pServer->clients[pServer->nrOfClients].data.name);
    pServer->pClientText[clientIndex] = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, buffer, pServer->windowWidth / 2, pServer->clients[pServer->nrOfClients].data.id * pServer->fontSize + 3 * pServer->fontSize);
    pServer->updateScreenFlag = 1;
    pServer->nrOfClients++;
    pServer->tcpState++;
}

void sendPlayerData(Server *pServer)
{
    for (int i = 0; i < pServer->nrOfClients - 1; i++)
    {
        SDLNet_TCP_Send(pServer->clients[pServer->nrOfClients - 1].tcpSocket, &pServer->clients[i].data, sizeof(Player));
        printf("Sent Playerdata of id: %d to new player\n", pServer->clients[i].data.id);
        SDLNet_TCP_Send(pServer->clients[i].tcpSocket, &pServer->clients[pServer->nrOfClients - 1].data, sizeof(Player));
    }
}

void updateTCPStateText(Server *pServer)
{
    char buffer[31];
    switch (pServer->tcpState)
    {
    case IDLE:
        strcpy(buffer, "Server is IDLE");
        break;
    case CLIENT_JOINING:
        strcpy(buffer, "CLIENT IS JOINING");
        break;
    case SENDING_MAP:
        strcpy(buffer, "SENDING MAP TO CLIENT");
        break;
    case SENDING_PLAYER_ID:
        strcpy(buffer, "SENDING PLAYER ID");
        break;
    case GET_PLAYER_DATA:
        strcpy(buffer, "WAITING FOR PLAYER DATA");
        break;
    case SEND_NEW_PLATER_DATA:
        strcpy(buffer, "SENDING DATA TO NEW PLAYER");
        break;
    }
    printf("Server state changed to: %s\n", buffer);
    if (pServer->pServerStateText)
        freeText(pServer->pServerStateText);
    pServer->pServerStateText = createText(pServer->pRenderer, 0, 0, 0, pServer->pFont, buffer, pServer->windowWidth / 2, pServer->fontSize);
    pServer->progressBar.w = pServer->fontSize * pServer->tcpState;
    if (pServer->tcpState == IDLE)
        pServer->progressBar.w = 0;
    pServer->updateScreenFlag = 1;
}

void checkIncommingTCP(Server *pServer)
{
    pthread_t mapThread;
    int bytesSent, bytesRecv, prevState = pServer->tcpState;

    checkTCPTimeout(pServer);

    switch (pServer->tcpState)
    {
    case IDLE:
        checkTCPForNewConnections(pServer);
        break;
    case CLIENT_JOINING:
        pthread_create(&mapThread, NULL, sendMapToPlayer, pServer); // start sending map
        pServer->tcpState++;
    case SENDING_MAP:
        if (pServer->mapPos == MAPSIZE * MAPSIZE) // check if server is done sending map
        {
            pthread_join(mapThread, NULL);
            occupySpawntile(pServer);
            pServer->tcpState++;
        }
        break;
    case SENDING_PLAYER_ID:
        sendPlayerId(pServer);
        break;
    case GET_PLAYER_DATA:
        while (SDLNet_CheckSockets(pServer->socketSetTCP, 0) > 0)
        {
            for (int i = 0; i < pServer->nrOfClients + 1; i++)
            {
                if (SDLNet_SocketReady(pServer->clients[i].tcpSocket))
                {
                    getPlayerData(pServer, i);
                }
            }
        }
        break;

    case SEND_NEW_PLATER_DATA:
        sendPlayerData(pServer);
        pServer->tcpState = IDLE;
        printf("DONE: Player joining complete!\n");
        break;
    }
    if (prevState != pServer->tcpState)
    {
        updateTCPStateText(pServer);
    }
}

void checkIncommingUDP(Server *pServer)
{
    if (SDLNet_UDP_Recv(pServer->socketUDP, pServer->pRecieve) == 1)
    {
        PlayerUdpPkg data;
        int id;
        static int hp[MAX_PLAYERS] = {0};
        /*
        Ta emot mindre(i bytes) structar som innehåller bara x y riktning och id
        */
        memcpy(&data, pServer->pRecieve->data, sizeof(PlayerUdpPkg));
        checkUDPClient(pServer, data);

        for (int i = 0; i < pServer->nrOfClients; i++)
            if (data.id == pServer->clients[i].id) {
                pServer->clients[i].timeout = SDL_GetTicks();
                pServer->clients[i].data.x = data.x;
                pServer->clients[i].data.y = data.y;
                pServer->clients[i].data.prevKeyPressed = data.direction;
                pServer->clients[i].data.idle = data.idle;
                pServer->clients[i].data.charging = data.charging;
                pServer->clients[i].data.charge = data.charge;
                pServer->clients[i].data.state = data.state;
                pServer->clients[i].data.hp = data.hp;
                id = i;
            }

        chargingCollisions(pServer, id);    
        data.hp = pServer->clients[id].data.hp < 0 ? 0 : pServer->clients[id].data.hp;    

        for (int i = 0; i < pServer->nrOfClients; i++)
        {
            if (pServer->clients[i].address.port != 8888)
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
}

void checkUDPClient(Server *pServer, PlayerUdpPkg data)
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
                printf("New udp client joined!\n");
                return;
            }
        }
    }
}

void updateServerWindow(Server *pServer)
{
    if (pServer->updateScreenFlag)
    {
        pServer->updateScreenFlag = 0;
        SDL_SetRenderDrawColor(pServer->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pServer->pRenderer);

        for (int i = 0; i < pServer->nrOfClients; i++)
            if (pServer->pClientText[i])
                drawText(pServer->pClientText[i], pServer->pRenderer);

        if (pServer->pServerStateText)
            drawText(pServer->pServerStateText, pServer->pRenderer);

        SDL_SetRenderDrawColor(pServer->pRenderer, 0, 255, 0, 255);
        SDL_RenderFillRect(pServer->pRenderer, &pServer->progressBar);

        SDL_RenderPresent(pServer->pRenderer);
    }
}

void dbgPrint()
{
    printf("Error: %s\n", SDL_GetError());
}

void closeS(Server *pServer)
{
    printf("Closing server...\n");
    if (pServer->pServerStateText)
        freeText(pServer->pServerStateText);
    printf("Closing clients...\n");
    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        SDLNet_TCP_Close(pServer->clients[i].tcpSocket);
        SDLNet_DelSocket(pServer->socketSetTCP, (SDLNet_GenericSocket)pServer->clients[i].tcpSocket);
    }
    printf("DONE: Closing clients\n");

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

    for (int i = 0; i < MAX_PLAYERS; i++)
        if (pServer->pClientText[i])
            freeText(pServer->pClientText[i]);

    if (pServer->pJoining)
        freeText(pServer->pJoining);

    if (pServer->pIP)
        freeText(pServer->pIP);

    if (pServer->pFont)
        TTF_CloseFont(pServer->pFont);

    TTF_Quit();

    SDLNet_Quit();

    SDL_Quit();
    printf("DONE: Closing server\n");
}