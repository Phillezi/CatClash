#include <SDL2/SDL_net.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#define PORT 1234

void *scanAndSend(void *socket);

int main(int argc, char **argv)
{
    pthread_t thread1;

    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        return -1;
    }

    UDPsocket socket = SDLNet_UDP_Open(0);
    if (!socket)
    {
        printf("SDLNet_UDP_Open failed: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return -1;
    }

    IPaddress address;
    if (SDLNet_ResolveHost(&address, "localhost", PORT) < 0)
    {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(socket);
        SDLNet_Quit();
        return -1;
    }

    UDPpacket *packet = SDLNet_AllocPacket(512);
    if (!packet)
    {
        printf("SDLNet_AllocPacket failed: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(socket);
        SDLNet_Quit();
        return -1;
    }

    int count = 0, nrRecv = 0;

    pthread_create(&thread1, NULL, scanAndSend, (void *)socket);
    while (1)
    {
        nrRecv = SDLNet_UDP_Recv(socket, packet);
        if (nrRecv < 0)
        {
            printf("SDLNet_UDP_Send failed: %s\n", SDLNet_GetError());
            SDLNet_FreePacket(packet);
            SDLNet_UDP_Close(socket);
            SDLNet_Quit();
            return -1;
        }
        if (nrRecv)
            printf("Received packet from %s:%d: %s\n", SDLNet_ResolveIP(&packet->address), packet->address.port, packet->data);
    }
    pthread_join(thread1, NULL);
    
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(socket);
    SDLNet_Quit();

    return 0;
}

void *scanAndSend(void *socket)
{
    IPaddress address;
    if (SDLNet_ResolveHost(&address, "localhost", PORT) < 0)
    {
        printf("SDLNet_ResolveHost failed: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close((UDPsocket )socket);
        SDLNet_Quit();
        return NULL;
    }

    UDPpacket *packet = SDLNet_AllocPacket(512);
    while (1)
    {
        scanf(" %s", (char *)packet->data);
        // strcpy((char *)packet->data, "Hello, world!");
        packet->len = strlen((char *)packet->data) + 1;
        packet->address.host = address.host;
        packet->address.port = address.port;

        if (SDLNet_UDP_Send((UDPsocket )socket, -1, packet) < 0)
        {
            printf("SDLNet_UDP_Send failed: %s\n", SDLNet_GetError());
            SDLNet_FreePacket(packet);
            SDLNet_UDP_Close((UDPsocket )socket);
            SDLNet_Quit();
            return NULL;
        }
        printf("Message sent!\n");
    }
}