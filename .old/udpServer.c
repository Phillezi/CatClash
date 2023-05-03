#include <SDL2/SDL_net.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char **argv)
{

    SDL_Window *pWindow = SDL_CreateWindow("UDP SERVER", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 200, 0);
    if (!pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init failed: %s\n", SDLNet_GetError());
        return 1;
    }

    UDPsocket socket = SDLNet_UDP_Open(1234);
    if (!socket)
    {
        printf("SDLNet_UDP_Open failed: %s\n", SDLNet_GetError());
        SDLNet_Quit();
        return 1;
    }

    UDPpacket *packet = SDLNet_AllocPacket(512);
    if (!packet)
    {
        printf("SDLNet_AllocPacket failed: %s\n", SDLNet_GetError());
        SDLNet_UDP_Close(socket);
        SDLNet_Quit();
        return 1;
    }

    int running = 1;

    while (running)
    {
        int numrecv = SDLNet_UDP_Recv(socket, packet);
        if (numrecv == -1)
        {
            printf("SDLNet_UDP_Recv failed: %s\n", SDLNet_GetError());
            running = 0;
            break;
        }
        else if (numrecv)
        {
            printf("Received packet from %s:%d: %s\n", SDLNet_ResolveIP(&packet->address), packet->address.port, packet->data);
            
            if (strcmp((char *)packet->data, "quit") == 0)
            {
                printf("Quitting...\n");
                running = 0;
                break;
            }
            SDLNet_UDP_Send(socket, -1, packet);
        }
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
                printf("CLOSING UPON REQUEST\n");
                break;
            }
        }

        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pRenderer);
        SDL_RenderPresent(pRenderer);
    }

    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(socket);
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}