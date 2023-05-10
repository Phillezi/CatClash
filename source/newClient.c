#include "../include/newClient.h"
#include "../include/player.h"
#include "../include/getDefaultGateway.h"
#include "semaphore.h"

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
    pkg.hp = pGame->pPlayer->hp;
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
            else if (pGame->pPlayer->id == tmp.id) {
                pGame->pPlayer->hp = tmp.hp;
                printf("Recived hp: %d\n", pGame->pPlayer->hp);
            }
        }
    }
}

struct netscantcp
{
    IPaddress ip;
    Uint8 connected;
    pthread_t threadId;
    sem_t started;
    sem_t done;
};
typedef struct netscantcp NetScanTcp;

void *tryOpenIp(void *pNetScanIn)
{
    NetScanTcp *pNet = (NetScanTcp *)pNetScanIn;
    pNet->connected = 0;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    sem_post(&pNet->started);
    TCPsocket socket = SDLNet_TCP_Open(&pNet->ip);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if (socket != NULL)
    {
        Uint8 joinFlag = 0;
        int bytesSend = SDLNet_TCP_Send(socket, &joinFlag, sizeof(joinFlag));
        printf("___Server found at %d:%d____\n", pNet->ip.host, pNet->ip.port);
        SDLNet_TCP_Close(socket);
        pNet->connected = 1;
    }
    return (NULL);
}

void *timeoutIpThread(void *pNetScanIn)
{
    NetScanTcp *pNet = (NetScanTcp *)pNetScanIn;
    sem_wait(&pNet->started);
    //SDL_Delay(100);
    if (_pthread_tryjoin(pNet->threadId, NULL) != 0)
    {
        if (pthread_cancel(pNet->threadId) == 0)
            ;
    }
    else
        printf("Thread was done\n");
    sem_post(&pNet->done);
    pthread_exit(NULL);
}

void *scanForGamesOnLocalNetwork(void *arg)
{
    LocalServer *pLocalServer = (LocalServer *)arg;
    bool found_ip = false;

    char ipStr[16], defaultGateway[16];

    getDefaultGateway(defaultGateway);
    int pos = 0;
    for (int i = strlen(defaultGateway) - 1; i > strlen(defaultGateway) - 1 - 3; i--)
        if (defaultGateway[i] == '.')
        {
            pos = i + 1;
            break;
        }

    Uint8 startval = 0;
    for (pos; pos < strlen(defaultGateway); pos++)
    {
        startval = startval * 10 + defaultGateway[pos] - (int)'0' + 1;
        defaultGateway[pos] = 0;
    }

    // printf("Default Gateway: %s\n", defaultGateway);

    NetScanTcp net[255 - startval];
    pthread_t tryOpenThread[255 - startval], timeoutThread[255 - startval];
    for (int i = startval; i < 255; i++)
    {
        sprintf(ipStr, "%s%d", defaultGateway, i);
        printf("Trying: %s\n", ipStr);
        if (SDLNet_ResolveHost(&net[i - startval].ip, ipStr, 1234) != 0)
            printf("Could not resolve host\n");
        else
        {
            net[i - startval].threadId = tryOpenThread[i - startval];
            sem_init(&net[i - startval].started, 0, 0);
            sem_init(&net[i - startval].done, 0, 0);

            if (pthread_create(&tryOpenThread[i - startval], NULL, tryOpenIp, &net[i - startval]) != 0)
                printf("Error creating tryOpen thread\n");
            else if (pthread_create(&timeoutThread[i - startval], NULL, timeoutIpThread, &net[i - startval]) != 0)
                printf("Error creating TimeOut thread\n");
        }
    }

    // SDL_Delay(2000); // sleep a little and let the threads work

    for (int i = startval; i < 255; i++)
    {
        sem_wait(&net[i - startval].done);
        if (_pthread_tryjoin(tryOpenThread[i - startval], NULL) != 0)
            pthread_cancel(tryOpenThread[i - startval]);

        if (net[i - startval].connected)
        {
            if (!pLocalServer->ppIpStringList)
            {
                printf("Allocating Memory for string array containing open IPs\n");
                pLocalServer->ppIpStringList = (char **)malloc(sizeof(char *));
                if (!pLocalServer->ppIpStringList)
                    printf("FAILED: When allocating Memory for string array containing open IPs\n");
            }
            else
            {
                printf("Expanding string array for new open ip string\n");
                pLocalServer->ppIpStringList = (char **)realloc(pLocalServer->ppIpStringList, pLocalServer->nrOfServersFound + 1 * sizeof(char *));
            }
            printf("Calloc memory for string in string array\n");
            pLocalServer->ppIpStringList[pLocalServer->nrOfServersFound] = (char *)calloc(16, sizeof(char));
            if (!pLocalServer->ppIpStringList[pLocalServer->nrOfServersFound])
                printf("FAILED: When callocing memory for string in string array\n");
            else
            {
                sprintf(pLocalServer->ppIpStringList[pLocalServer->nrOfServersFound], "%s%d", defaultGateway, i);
                pLocalServer->nrOfServersFound++;
                pLocalServer->foundServer = true;
            }
        }
        if (_pthread_tryjoin(timeoutThread[i - startval], NULL) != 0)
            pthread_cancel(timeoutThread[i - startval]);
    }
    /*if (found_ip_index != -1)
    {
        sprintf(pLocalServer->ipString, "%s%d", defaultGateway, found_ip_index);
        pLocalServer->foundServer = true;
    }
    else
    {
        pLocalServer->foundServer = false;
    }*/

    pLocalServer->searchDone = true; // set done with scan flag to true
    return NULL;
}