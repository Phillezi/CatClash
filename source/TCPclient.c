#include "TCPclient.h"
#include <string.h>

/*
    joinServerTCP
    This function retrives a map from a server hosted at the adress given by pGame->pClient->serverIP
*/
int joinServerTCP(Game *pGame)
{
    
    if (SDLNet_ResolveHost(&pGame->pClient->ip, pGame->pClient->serverIP, pGame->pClient->port) == -1)
        return 1;

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(1);
    pGame->pClient->socketTCP = SDLNet_TCP_Open(&pGame->pClient->ip);
    
    if (!pGame->pClient->socketTCP)
    {
        SDLNet_FreeSocketSet(sockets);
        return 1;
    }
    SDLNet_TCP_AddSocket(sockets, pGame->pClient->socketTCP);
    bool exit = false;

    int i = 0;

    while (!exit)
    {
        while (SDLNet_CheckSockets(sockets, 0) > 0)
        {
            if (SDLNet_SocketReady(pGame->pClient->socketTCP))
            {
                int bytesRecv = SDLNet_TCP_Recv(pGame->pClient->socketTCP, &pGame->map[i].type, sizeof(pGame->map[i].type));
                i++;
                if (bytesRecv != sizeof(pGame->map[i].type))
                {
                    printf("PACKET LOSS\n");
                    exit = true;
                    SDLNet_TCP_Close(pGame->pClient->socketTCP);
                    SDLNet_FreeSocketSet(sockets);
                    return 1;
                }
                if (i == MAPSIZE * MAPSIZE)
                {
                    printf("Recieved message\n");
                    exit = true;
                    SDLNet_TCP_Close(pGame->pClient->socketTCP);
                    SDLNet_FreeSocketSet(sockets);
                }
            }
        }
    }
    printf("Closing TCP Client, and opening udp client...\n");
    //pGame->pClient->socketUDP = SDLNet_UDP_Open(1234);
    return 0;
    // SDLNet_TCP_Close(pGame->pClient->socketTCP);
}

void sendDataUDP(Game *pGame){
    printf("1\n");
    PacketData packet;// = {pGame->pPlayer->x, pGame->pPlayer->y, pGame->pPlayer->id};
    packet.id = pGame->pPlayer->id;
    packet.x = pGame->pPlayer->x;
    packet.y = pGame->pPlayer->y;
    //pGame->pPacket = malloc(sizeof(packet));
    //memcpy(pGame->pPacket->data, &packet, sizeof(PacketData));

    UDPpacket packetS;
    memcpy(&packetS.data, &packet, sizeof(packet)+1);

    packetS.address.host = pGame->pClient->ip.host;
    packetS.address.port = pGame->pClient->ip.port;

    if(!SDLNet_UDP_Send(pGame->pClient->socketUDP, -1, &packetS)){
        printf("ERROR: could not send package\n");
        printf("Error: %S\n", SDLNet_GetError());
    }

    //pGame->pPacket->len = sizeof(PacketData)+1;

    //pGame->pPacket->address.host = pGame->pClient->ip.host;	// Set the destination host 
    //pGame->pPacket->address.port = pGame->pClient->ip.port;	// And destination port 
    /*
    if(!SDLNet_UDP_Send(pGame->pClient->socketUDP, -1, pGame->pPacket)){
        printf("ERROR: could not send package\n");
    }
    */
    //free(pGame->pPacket);
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