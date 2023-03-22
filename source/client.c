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
    char text[100];
    SDLNet_TCP_Recv(client,text,99);
    printf("MESSAGE: %s\n", text);

    SDLNet_TCP_Close(client);

    SDLNet_Quit();
    SDL_Quit;
}