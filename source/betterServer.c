#include "definitions.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "init.h"
#define PORT 1234

int initServer(Server *pServer);
void checkIncommingTCP(Server *pServer);
void checkIncommingUDP(Server *pServer);
void checkUDPClient(Server *pServer, PlayerUdpPkg data);
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
        }
    }

    closeS(&server);
    return 1;
}

int initServer(Server *pServer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        dbgPrint();
        return 1;
    }
    if (SDLNet_Init())
    {
        dbgPrint();
        return 1;
    }

    pServer->nrOfClients = 0;

    // SETUP TCP
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

    // SETUP UDP
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

    if (initMap(pServer->map, "map", 16))
        return 1;

    return 0;
}

void checkIncommingTCP(Server *pServer)
{
    int bytesSent, bytesRecv, prevState = pServer->tcpState;
    if (pServer->tcpState == IDLE)
    {
        TCPsocket tmpClient = SDLNet_TCP_Accept(pServer->socketTCP);
        if (tmpClient)
        {
            printf("TCP ACCEPTED\n");
            if (pServer->nrOfClients < MAX_PLAYERS)
            {
                pServer->clients[pServer->nrOfClients].tcpSocket = tmpClient;
                SDLNet_TCP_AddSocket(pServer->socketSetTCP, pServer->clients[pServer->nrOfClients].tcpSocket);
                printf("User: %d joined!\n", pServer->nrOfClients);
                // SDLNet_TCP_Close(tmpClient);
                pServer->tcpState = CLIENT_JOINING;
            }
        }
        for (int i = 0; i < pServer->nrOfClients; i++)
        {
            if (SDL_GetTicks() - pServer->clients[i].timeout > 5000)
            {
                printf("Player %d timed out\n Disconnection them\n", i);
                SDLNet_TCP_Close(pServer->clients[i].tcpSocket);
                SDLNet_DelSocket(pServer->socketSetTCP, pServer->clients[i].tcpSocket);
                for (i; i < pServer->nrOfClients; i++)
                {
                    if (i + 1 < MAX_PLAYERS)
                        pServer->clients[i] = pServer->clients[i + 1];
                    else
                    {
                        pServer->clients[i].address.port = 8888;
                        pServer->clients[i].address.host = 8888;
                        pServer->clients[i].id = 8888;
                    }
                }
                pServer->nrOfClients--;
            }
        }
    }

    switch (pServer->tcpState)
    {
    case IDLE:
        break;
    case CLIENT_JOINING:
        pServer->mapPos = 0;
        pServer->tcpState++;
    case SENDING_MAP:
        bytesSent = SDLNet_TCP_Send(pServer->clients[pServer->nrOfClients].tcpSocket, &pServer->map[pServer->mapPos].type, sizeof(pServer->map[pServer->mapPos].type)); // send map to client
        if (bytesSent != sizeof(pServer->map[pServer->mapPos].type))
        {
            printf("Error: packet loss when sending map nr:%d\n", pServer->mapPos);
        }
        pServer->mapPos++;
        if (pServer->mapPos == MAPSIZE * MAPSIZE)
            pServer->tcpState++;
        break;
    case SENDING_PLAYER_ID:
        bytesSent = SDLNet_TCP_Send(pServer->clients[pServer->nrOfClients].tcpSocket, &pServer->nrOfClients, sizeof(pServer->nrOfClients));
        if (bytesSent != sizeof(pServer->nrOfClients))
        {
            printf("Error: packet loss when sending player id\n");
        }
        pServer->tcpState++;
        break;
    case GET_PLAYER_DATA:
        while (SDLNet_CheckSockets(pServer->socketSetTCP, 0) > 0)
        {
            for (int i = 0; i < pServer->nrOfClients + 1; i++)
            {
                if (SDLNet_SocketReady(pServer->clients[i].tcpSocket))
                {
                    pServer->clients[i].timeout = SDL_GetTicks();
                    bytesRecv = SDLNet_TCP_Recv(pServer->clients[pServer->nrOfClients].tcpSocket, &pServer->clients[pServer->nrOfClients].data, sizeof(Player));
                    if (bytesRecv != sizeof(Player))
                    {
                        printf("Error when reciving Player struct over TCP\n");
                        dbgPrint();
                    }
                    if (pServer->clients[pServer->nrOfClients].data.id != pServer->nrOfClients)
                    {
                        printf("Error: Client Recived client isnt the same as joined client");
                    }
                    pServer->nrOfClients++;
                    pServer->tcpState++;
                }
            }
        }
        break;

    case SEND_NEW_PLATER_DATA:
        for (int i = 0; i < pServer->nrOfClients - 1; i++)
        {
            SDLNet_TCP_Send(pServer->clients[pServer->nrOfClients - 1].tcpSocket, &pServer->clients[i].data, sizeof(Player));
            printf("Sent Playerdata of id: %d to new player\n", pServer->clients[i].data.id);
            SDLNet_TCP_Send(pServer->clients[i].tcpSocket, &pServer->clients[pServer->nrOfClients - 1].data, sizeof(Player));
        }
        pServer->tcpState = IDLE;
        printf("DONE: Player joining complete!\n");
        break;
    }
    if (prevState != pServer->tcpState)
    {
        printf("Server state changed to: %d\n", pServer->tcpState);
    }
}

void checkIncommingUDP(Server *pServer)
{
    if (SDLNet_UDP_Recv(pServer->socketUDP, pServer->pRecieve) == 1)
    {
        PlayerUdpPkg data;
        /*
        Ta emot mindre(i bytes) structar som innehåller bara x y riktning och id
        */
        memcpy(&data, pServer->pRecieve->data, sizeof(PlayerUdpPkg));
        checkUDPClient(pServer, data);
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
            else if (pServer->clients[i].data.id == data.id)
            {
                pServer->clients[i].timeout = SDL_GetTicks();
                pServer->clients[i].data.x = data.x;
                pServer->clients[i].data.y = data.y;
                pServer->clients[i].data.prevKeyPressed = data.direction;
                // pServer->clients[i].data.idle = data.idle;
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

void dbgPrint()
{
    printf("Error: %s\n", SDL_GetError());
}

void closeS(Server *pServer)
{

    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        SDLNet_TCP_Close(pServer->clients[i].tcpSocket);
        SDLNet_DelSocket(pServer->socketSetTCP, pServer->clients[i].tcpSocket);
    }

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

    SDL_Quit();
}