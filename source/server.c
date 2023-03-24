// #include "server.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>

#define MAX_SOCKETS 5
typedef struct data
{
    TCPsocket socket;
    Uint32 timeout;
    int playerId
} Data;

int main(int argc, char **argv)
{
    // SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    int playerCount = 0, playerId = 0;

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, 1234);

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(MAX_SOCKETS);

    TCPsocket server = SDLNet_TCP_Open(&ip);
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
                    
                    int num = tmp[0] - '0';
                    int j = 1;
                    while (tmp[j] >= '0' && tmp[j] <= '9')
                    {
                        num *= 10;
                        num += tmp[j] - '0';
                        j++;
                    }
                    if (num == 1)
                    {
                        printf("Player%d: %s\n", player[i].playerId, tmp);
                        for (int k = 0; k < playerCount; k++)
                        {
                            if (k == i)
                            {
                                continue;
                            }
                            SDLNet_TCP_Send(player[k].socket, tmp, strlen(tmp) + 1);
                        }
                    }
                    else if (num == 2)
                    {
                        printf("(LEAVE)Player%d: %s\n", player[i].playerId, tmp);
                        for (int k = 0; k < playerCount; k++)
                        {
                            if (k == i)
                            {
                                continue;
                            }
                            SDLNet_TCP_Send(player[k].socket, tmp, strlen(tmp) + 1);
                        }
                        SDLNet_TCP_DelSocket(sockets, player[i].socket);
                        SDLNet_TCP_Close(player[i].socket);
                        playerCount--;
                    }
                }
            }
        }
        SDL_Delay(1);
    }
    for(int i = 0; i < playerCount; i++){
        SDLNet_TCP_Close(player[i].socket);
    }

    SDLNet_FreeSocketSet(sockets);
    SDLNet_TCP_Close(server);

    SDLNet_Quit();
    SDL_Quit;
}