#include "newClient.h"
#include "player.h"
#include "getDefaultGateway.h"

enum TCPSTATE
{
    FETCHING_MAP,
    FETCHING_ID
};

int connectToServer(Game *pGame)
{
    if (initTCPConnection(pGame))
    {
        printf("Error: Initialization of tcp connection failed\n");
        return 1;
    }
    printf("DONE: Initialized TCP\n");
    enum TCPSTATE tcpState = FETCHING_MAP;
    bool exit = false;
    int i = 0;
    int bytesRecv, bytesSent;

    while (!exit)
    {
        if (SDLNet_CheckSockets(pGame->pClient->sockets, 0) > 0)
        {
            if (SDLNet_SocketReady(pGame->pClient->socketTCP))
            {
                switch (tcpState)
                {
                case FETCHING_MAP:
                    bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, &pGame->map[i].type, sizeof(pGame->map[i].type));

                    if (bytesRecv != sizeof(pGame->map[i].type))
                    {
                        printf("Error: Packet loss when reciving mapdata nr:%d\n", i);
                        exit = true;
                        return 1;
                    }
                    i++;
                    if (i == MAPSIZE * MAPSIZE)
                        tcpState++;
                    break;
                case FETCHING_ID:
                    bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, &pGame->pPlayer->id, sizeof(pGame->pPlayer->id));
                    if (bytesRecv != sizeof(pGame->pPlayer->id))
                    {
                        printf("Error: Packet loss when reciving player id\n");
                    }
                    bytesSent = SDLNet_TCP_Send(pGame->pClient->socketTCP, pGame->pPlayer, sizeof(Player));
                    if (bytesSent != sizeof(Player))
                    {
                        printf("Error: Error when sending Player structure over TCP\n");
                        exit = true;
                        SDLNet_TCP_Close(pGame->pClient->socketTCP);
                        SDLNet_FreeSocketSet(pGame->pClient->sockets);
                        return 1;
                    }
                    exit = true;
                    pGame->isConnected = true;
                    tcpState++;
                    break;
                }
            }
        }
    }
    return 0;
}

int initTCPConnection(Game *pGame)
{
    pGame->nrOfPlayers = 0;

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, pGame->pClient->serverIP, pGame->pClient->port) == -1)
    {
        printf("Error: could not resolve host\n");
        return 1;
    }

    pGame->pClient->sockets = SDLNet_AllocSocketSet(1);
    pGame->pClient->socketTCP = SDLNet_TCP_Open(&ip);
    if (!pGame->pClient->socketTCP)
    {
        printf("Error: could not open Session\n");
        SDLNet_FreeSocketSet(pGame->pClient->sockets);
        return 1;
    }
    SDLNet_TCP_AddSocket(pGame->pClient->sockets, pGame->pClient->socketTCP);

    Uint8 joinFlag = 1;
    int bytesSend = SDLNet_TCP_Send(pGame->pClient->socketTCP, &joinFlag, sizeof(joinFlag));
    if (bytesSend != sizeof(joinFlag))
    {
        printf("Error: could not join server :(\n");
        SDLNet_FreeSocketSet(pGame->pClient->sockets);
        SDLNet_TCP_DelSocket(pGame->pClient->sockets, pGame->pClient->socketTCP);
        return 1;
    }

    return 0;
}

void checkTCP(Game *pGame)
{
    Player data;
    if (SDLNet_CheckSockets(pGame->pClient->sockets, 0) > 0)
    {
        if (SDLNet_SocketReady(pGame->pClient->socketTCP))
        {
            int bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, &data, sizeof(Player));
            if (bytesRecv != sizeof(Player))
            {
                printf("Error: Packetloss when reciving Playerdata over TCP\n");
                printf("Recived: x:%d, y:%d, id:%d, name:%s\n", data.x, data.y, data.id, data.name);
            }
            else
            {
                if (data.disconnectedFlag == 1)
                {
                    printf("Recived Disconnect message\n");
                    for (int i = 0; i < pGame->nrOfPlayers; i++)
                    {
                        if (data.id == pGame->pMultiPlayer[i].id)
                        {
                            printf("Disconnected player is: %d index in Player array: %d\n", pGame->pMultiPlayer[i].id, i);
                            for (i; i < pGame->nrOfPlayers - 1; i++)
                            {
                                if (i + 1 < pGame->nrOfPlayers)
                                {
                                    pGame->pMultiPlayer[i] = pGame->pMultiPlayer[i + 1];
                                }
                            }
                            printf("Removed player %d\n", data.id);
                            pGame->nrOfPlayers--;
                            pGame->pMultiPlayer = removePlayer(pGame, pGame->nrOfPlayers);
                            // destroyPlayer(&pGame->pMultiPlayer[i]); // venne om de här funkar
                        }
                    }
                }
                else
                {
                    pGame->pMultiPlayer = createNewMultiPlayer(pGame, pGame->nrOfPlayers, data);
                    pGame->nrOfPlayers++;
                    printf("Recived: x:%d, y:%d, id:%d, name:%s\n", data.x, data.y, data.id, data.name);
                    printf("A new player joined! (%d total)\n", pGame->nrOfPlayers);
                }
            }
        }
    }
}

PlayerNet *createClient(char *serverIP, int port, int id, int x, int y)
{
    PlayerNet *pClient = malloc(sizeof(PlayerNet));
    if (!pClient)
    {
        printf("Error: Failed to allocate memory for playerNet structure\n");
        return NULL;
    }
    strcpy(pClient->serverIP, serverIP);
    pClient->port = port;
    pClient->id = id;
    pClient->x = x;
    pClient->y = y;
    return pClient;
}

// UDP

void sendData(Game *pGame)
{
    PlayerUdpPkg pkg;
    pkg.id = pGame->pPlayer->id;
    pkg.idle = pGame->pPlayer->idle;
    pkg.x = pGame->pPlayer->x;
    pkg.y = pGame->pPlayer->y;
    pkg.direction = pGame->pPlayer->prevKeyPressed;
    pkg.charge = pGame->pPlayer->charge;
    pkg.charging = pGame->pPlayer->charging;
    pkg.state = pGame->pPlayer->state;
    memcpy(pGame->pPacket->data, &pkg, sizeof(PlayerUdpPkg));

    pGame->pPacket->len = sizeof(PlayerUdpPkg);

    pGame->pPacket->address.host = pGame->serverAddress.host; // Set the destination host
    pGame->pPacket->address.port = pGame->serverAddress.port; // And destination port

    if (!SDLNet_UDP_Send(pGame->socketDesc, -1, pGame->pPacket))
    {
        pGame->isConnected = false;
        printf("Could not send package\n");
    }
}

void getPlayerData(Game *pGame)
{
    while (SDLNet_UDP_Recv(pGame->socketDesc, pGame->pPacket))
    {
        PlayerUdpPkg tmp;
        memcpy(&tmp, pGame->pPacket->data, sizeof(PlayerUdpPkg));

        for (int j = 0; j < pGame->nrOfPlayers; j++)
        {
            if (pGame->pMultiPlayer[j].id == tmp.id)
            {
                pGame->pMultiPlayer[j].x = tmp.x;
                pGame->pMultiPlayer[j].y = tmp.y;
                pGame->pMultiPlayer[j].idle = tmp.idle;
                pGame->pMultiPlayer[j].prevKeyPressed = tmp.direction;
                pGame->pMultiPlayer[j].charge = tmp.charge;
                pGame->pMultiPlayer[j].charging = tmp.charging;
                pGame->pMultiPlayer[j].state = tmp.state;
            }
        }
    }
}

void *tryOpenIp(void *pIpIn)
{
    IPaddress *pIp = (IPaddress *)pIpIn;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    TCPsocket socket = SDLNet_TCP_Open(pIp);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (socket != NULL)
    {
        Uint8 joinFlag = 0;
        int bytesSend = SDLNet_TCP_Send(socket, &joinFlag, sizeof(joinFlag));
        printf("Server found at %d:%d\n", pIp->host, pIp->port);
        SDLNet_TCP_Close(socket);
        pthread_exit((void *)1);
    }
    pthread_exit((void *)0);
}

void *timeoutIpThread(void *threadIDIn)
{
    pthread_t threadID = *(pthread_t *)threadIDIn;
    SDL_Delay(2000);
    printf("Thread %d timed out\nAttempting to cancel...\n", threadID);
    if(_pthread_tryjoin(threadID, NULL) != 0)
    {
        pthread_cancel(threadID);
        printf("Thread cancelled!\n");
    }  
    else
        printf("Thread was done\n");
    
    pthread_exit(NULL);
}

void *scanForGamesOnLocalNetwork(void *arg)
{
    bool *pDoneFlag = (bool *)arg, found_ip = false;
    IPaddress ip;
    char ipStr[16], defaultGateway[16];
    void *pThread_Result, *pFound_Ip;

    getDefaultGateway(defaultGateway);
    Uint8 startval = defaultGateway[strlen(defaultGateway) - 1] - (int)'0' + 1;
    printf("Default Gateway: %s\n", defaultGateway);
    defaultGateway[strlen(defaultGateway) - 1] = 0;

    pthread_t tryOpenThread[255-startval], timeoutThread[255-startval];
    for (int i = startval; i < 255; i++)
    {
        sprintf(ipStr, "%s%d", defaultGateway, i);
        printf("Trying: %s\n", ipStr);
        SDLNet_ResolveHost(&ip, ipStr, 1234);
        pthread_create(&tryOpenThread[i-startval], NULL, tryOpenIp, &ip);
        pthread_create(&timeoutThread[i-startval], NULL, timeoutIpThread, &tryOpenThread[i-startval]);
    }
    int found_ip_index = -1;
    for (int i = startval; i < 255; i++)
    {
        pthread_join(tryOpenThread[i-startval], &pThread_Result);
        if (*(int *)pThread_Result)
        {
            found_ip_index = i;
        }
        pthread_join(timeoutThread[i-startval], NULL);
        printf("Done: Thread:%d\n", i);
    }
    if (found_ip_index != -1)
    {
        printf("Server found at: %s%d\n", defaultGateway, found_ip_index);
    }
    else
    {
        printf("Server not found on local network.\n");
    }

    printf("Done with scan\n");
    *pDoneFlag = true; // set done with scan flag to true
    pthread_exit(NULL);
}