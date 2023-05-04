#include "TCPclient.h"
#include "player.h"

/*
    joinServerTCP
    This function retrives a map from a server hosted at the adress given by pGame->pClient->serverIP
*/
int joinServerTCP(Game *pGame)
{
    pGame->nrOfPlayers = 0;

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, pGame->pClient->serverIP, pGame->pClient->port) == -1)
        return 1;

    pGame->pClient->sockets = SDLNet_AllocSocketSet(1);
    pGame->pClient->socketTCP = SDLNet_TCP_Open(&ip);
    if (!pGame->pClient->socketTCP)
    {
        SDLNet_FreeSocketSet(pGame->pClient->sockets);
        return 1;
    }
    SDLNet_TCP_AddSocket(pGame->pClient->sockets, pGame->pClient->socketTCP);
    bool exit = false;

    int i = 0, bytesRecv = 0, bytesSent = 0;
    Uint8 mapRecvFlag = 0;
    enum TCPSTATES
    {
        RECVMAP,
        RECVID,
        RECVDATA
    };
    enum TCPSTATES tcpState = RECVMAP;

    while (!exit)
    {
        while (SDLNet_CheckSockets(pGame->pClient->sockets, 0) > 0)
        {
            if (SDLNet_SocketReady(pGame->pClient->socketTCP))
            {
                switch (tcpState)
                {
                case RECVMAP:
                    bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, &pGame->map[i].type, sizeof(pGame->map[i].type));
                    i++;
                    if (bytesRecv != sizeof(pGame->map[i - 1].type))
                    {
                        printf("Packet loss when reciving mapdata nr:%d\n", (i - 1));
                        exit = true;
                        SDLNet_TCP_Close(pGame->pClient->socketTCP);
                        SDLNet_FreeSocketSet(pGame->pClient->sockets);
                        return 1;
                    }
                    if (i == MAPSIZE * MAPSIZE)
                    {
                        printf("DONE: reciving map\n");
                        /*
                        mapRecvFlag = 1;
                        bytesSent = SDLNet_TCP_Send(pGame->pClient->socketTCP, &mapRecvFlag, sizeof(Uint8));
                        if (bytesSent != sizeof(Uint8))
                        {
                            printf("Error: packet loss when sending mapRecvFlag\n");
                        }
                        printf("DONE: sending flag\n");
                        */
                        tcpState++;
                    }

                    break;
                case RECVID:
                    bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, &pGame->pPlayer->id, sizeof(int));
                    if (bytesRecv != sizeof(int))
                    {
                        printf("Packet loss when reciving player id\n");
                    }
                    printf("DONE: reciving playerID\n");
                    bytesSent = SDLNet_TCP_Send(pGame->pClient->socketTCP, pGame->pPlayer, sizeof(Player));
                    if (bytesSent != sizeof(Player))
                    {
                        printf("Error when sending Player structure over TCP\n");
                        exit = true;
                        SDLNet_TCP_Close(pGame->pClient->socketTCP);
                        SDLNet_FreeSocketSet(pGame->pClient->sockets);
                        return 1;
                    }
                    tcpState++;
                    //if (pGame->pPlayer->id <= 1)
                        exit = true;
                    break;
                case RECVDATA:
                    Player data;
                    SDLNet_TCP_Recv(pGame->pClient->socketTCP, &data, sizeof(Player));
                    if (data.id > pGame->nrOfPlayers)
                    {
                        pGame->pMultiPlayer = createNewMultiPlayer(pGame, pGame->nrOfPlayers, data);
                        pGame->nrOfPlayers++;
                        printf("A new player joined! (%d total)\n", pGame->nrOfPlayers);
                    }
                    if (pGame->pPlayer->id - pGame->nrOfPlayers == 1)
                        exit = true;
                    break;
                    // checkTcp(pGame);
                }
            }
        }
    }
    // printf("Closing TCP client...\n");
    return 0;
    // SDLNet_TCP_Close(pGame->pClient->socketTCP);
}

void checkTcp(Game *pGame)
{
    Player data;
    if (SDLNet_CheckSockets(pGame->pClient->sockets, 0) > 0)
    {
        if (SDLNet_SocketReady(pGame->pClient->socketTCP))
        {
            SDLNet_TCP_Recv(pGame->pClient->socketTCP, &data, sizeof(Player));
            if (data.id > pGame->nrOfPlayers)
            {
                pGame->pMultiPlayer = createNewMultiPlayer(pGame, pGame->nrOfPlayers, data);
                pGame->nrOfPlayers++;
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