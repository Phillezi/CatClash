#include "TCPclient.h"

int ANVEJservInit(Game *pGame)
{
    if (SDLNet_Init())
    {
        return 1;
    }
    pGame->pClient = createClient("localhost", 1234, 0, 100, 100);

    joinServerTCP(pGame);


    free(pGame->pClient);
    SDLNet_Quit();
    return 0;
}

/*
    joinServerTCP
    This function retrives a map from a server hosted at the adress given by pGame->pClient->serverIP
*/
int joinServerTCP(Game *pGame)
{
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, pGame->pClient->serverIP, pGame->pClient->port) == -1)
        return 1;

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(1);
    pGame->pClient->socketTCP = SDLNet_TCP_Open(&ip);
    if(!pGame->pClient->socketTCP){
        SDLNet_FreeSocketSet(sockets);
        return 1;
    }
    SDLNet_TCP_AddSocket(sockets, pGame->pClient->socketTCP);
    bool exit = false;
    SDLNet_TCP_Send(pGame->pClient->socketTCP, &pGame->world.tileSize, sizeof(pGame->world.tileSize));
    while (!exit)
    {
        while (SDLNet_CheckSockets(sockets, 0) > 0)
        {
            if (SDLNet_SocketReady(pGame->pClient->socketTCP))
            {
                int bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, pGame->map, sizeof(pGame->map));
                if(bytesRecv != sizeof(pGame->map)){
                    printf("PACKET LOSS");
                }
                    
                printf("Recieved message\n");
                exit = true;
                SDLNet_TCP_Close(pGame->pClient->socketTCP);
                SDLNet_FreeSocketSet(sockets);
            }
        }
    }
    printf("Closing client...\n");
    //SDLNet_TCP_Close(pGame->pClient->socketTCP);
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