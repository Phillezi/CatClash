#include <SDL2/SDL_net.h>
#define PORT 1234

int initClient(){
    if(SDL_Net_Init() < 0)
        return -1;
    UDPsocket client;
    IPaddress server;
    UDPpacket *pPacket;
    if(!(client = SDLNet_UDP_Open(0)))
        return -1;
    if(SDLNet_ResolveHost(&client, "localhost", PORT) < 0)
        return -1;
    SDLNet_UDP_Send(client, -1, pPacket);
}
int connectToServer(char *server, ){
    
}
