//#include "client.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>

#define SERVERIP "localhost"

int main(int argc, char **argv){
    //SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    IPaddress ip;
    SDLNet_ResolveHost(&ip, SERVERIP, 1234);

    TCPsocket client=SDLNet_TCP_Open(&ip);

    //TCPsocket server = SDLNet_AllocSocketSet(1);
    //SDLNet_AddSocket(server, client)

    char text[100];
    SDLNet_TCP_Recv(client,text,99);
    printf("MESSAGE: %s\n", text);
    char message[1400];
    scanf("1%s", message);
    SDLNet_TCP_Send(client, message, strlen(message)+1);
    strcpy(message,"2\n");
    SDLNet_TCP_Send(client, message, strlen(message)+1);

    SDLNet_TCP_Close(client);

    SDLNet_Quit();
    SDL_Quit;
}