#include "definitions.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>
#include "init.h"

#define MAX_SOCKETS 5
#define PORT 1234

int main(int argc, char **argv)
{
    SDLNet_Init();
    Tile map[1024];
    char mapName[100];
    do{
        printf("Which map do you want to host?\n:");
        scanf(" %100s", mapName);
    } while(initMap(map, mapName, 16));
    
    int playerCount = 0, playerId = 0;

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, PORT);

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(MAX_SOCKETS);

    TCPsocket server = SDLNet_TCP_Open(&ip);

    printf("Server is up and running\n");
    printf("Server is hosted at: %d:%d\n", ip.host, PORT);

    int running = 1;
    int tileSize = 0;
    while (running)
    {
        TCPsocket tmpClient = SDLNet_TCP_Accept(server);
        if (tmpClient)
        {
            if (playerCount < MAX_SOCKETS)
            {
                SDLNet_TCP_AddSocket(sockets, tmpClient);
                printf("User: %d joined!\n", playerId);
                for(int i = 0; i < MAPSIZE*MAPSIZE; i++)
                    SDLNet_TCP_Send(tmpClient, &map[i].type, sizeof(map[i].type)); // send map to client
                SDLNet_TCP_Close(tmpClient);                            // close client
                running = 0;
            }
            else
            {
            }

            //  break;
        }
    }
    SDLNet_FreeSocketSet(sockets);
    SDLNet_TCP_Close(server);

    SDLNet_Quit();
    SDL_Quit;
}