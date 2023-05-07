#include "newClient.h"
#include "player.h"

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
                pGame->pMultiPlayer = createNewMultiPlayer(pGame, pGame->nrOfPlayers, data);
                pGame->nrOfPlayers++;
                printf("Recived: x:%d, y:%d, id:%d, name:%s\n", data.x, data.y, data.id, data.name);
                printf("A new player joined! (%d total)\n", pGame->nrOfPlayers);
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
<<<<<<< Updated upstream
=======
    pkg.charging = pGame->pPlayer->charging;
    pkg.state = pGame->pPlayer->state;
    pkg.leavingFlag = 0;
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
                pGame->pMultiPlayer[j].x = tmp.x;
                pGame->pMultiPlayer[j].y = tmp.y;
                pGame->pMultiPlayer[j].idle = tmp.idle;
                pGame->pMultiPlayer[j].prevKeyPressed = tmp.direction;
                pGame->pMultiPlayer[j].charge = tmp.charge;
            }
            j = pGame->nrOfPlayers; // to end loop
        }
    }
}