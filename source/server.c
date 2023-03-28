/*
    TODO: fixa buggen om en klient lämnar så förstörs
    listan av spelare som man skickar meddelanden till
    fixa att den byter ut leave (2) meddelandet till Player(id) left för de som ej skrev Leave
    fixa client att om man tar emot (2) så lämnar man
*/

// #include "server.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>

#define MAX_SOCKETS 5
#define PORT 1234

typedef struct data
{
    TCPsocket socket;
    Uint32 timeout;
    int playerId
} Data;

int main(int argc, char **argv)
{
    SDLNet_Init();

    int playerCount = 0, playerId = 0;

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, PORT);

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(MAX_SOCKETS);

    TCPsocket server = SDLNet_TCP_Open(&ip);

    printf("Server is up and running\n");
    printf("Server is hosted at: %d:%d\n", ip.host, PORT);
    // TCPsocket client;
    char tmp[1400];
    Data player[MAX_SOCKETS];

    const char *text = "Hello World!\n";
    while (1)
    {
        TCPsocket tmpClient = SDLNet_TCP_Accept(server);
        if (tmpClient)
        {
            if (playerCount < MAX_SOCKETS)
            {
                SDLNet_TCP_AddSocket(sockets, tmpClient);
                player[playerCount].socket = tmpClient;
                player[playerCount].timeout = SDL_GetTicks();
                player[playerCount].playerId = playerId;

                printf("User: %d joined!\n", playerId);

                playerCount++;
                playerId++;
            }
            else
            {
            }
            SDLNet_TCP_Send(tmpClient, text, strlen(text) + 1);
            // SDLNet_TCP_Close(client);
            //  break;
        }
        while (SDLNet_CheckSockets(sockets, 0) > 0)
        {
            for (int i = 0; i < playerCount; i++)
            {
                if (SDLNet_SocketReady(player[i].socket))
                {
                    player[i].timeout = SDL_GetTicks();
                    SDLNet_TCP_Recv(player[i].socket, tmp, 1400);
                    char *msg = ("Player%d: %s", player[i].playerId, tmp);
                    if (tmp[0] == '2')
                    {
                        printf("Player%d has left the channel\n", player[i].playerId);
                        for (int k = 0; k < playerCount; k++)
                        {
                            if (k == i)
                            {
                                SDLNet_TCP_Send(player[k].socket, "2", 2);
                                continue;
                            }
                            char *leaveMsg = ("Player%d has left the channel", player[k].playerId);
                            SDLNet_TCP_Send(player[k].socket, leaveMsg, strlen(leaveMsg) + 1);
                        }
                        SDLNet_TCP_DelSocket(sockets, player[i].socket);
                        SDLNet_TCP_Close(player[i].socket);
                        playerCount--;
                    }
                    else
                    {
                        printf("Player%d: %s\n", player[i].playerId, tmp);
                        for (int k = 0; k < playerCount; k++)
                        {
                            if (k == i)
                            {
                                continue;
                            }
                            SDLNet_TCP_Send(player[k].socket, msg, strlen(msg) + 1);
                        }
                    }
                }
            }
        }
        SDL_Delay(1);
    }
    for (int i = 0; i < playerCount; i++)
    {
        SDLNet_TCP_Close(player[i].socket);
    }

    SDLNet_FreeSocketSet(sockets);
    SDLNet_TCP_Close(server);

    SDLNet_Quit();
    SDL_Quit;

    return 0;
}
