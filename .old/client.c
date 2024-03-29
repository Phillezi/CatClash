#include "client.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define SERVERIP "localhost"

void *readMessage(void *client);

int tryConnectTcp(char serverIP[])
{
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, serverIP, 1234) == -1)
        return 1;
    printf("%d\n",ip.host);
    return 0;
}

int clientTCP(char serverIP[])
{
    pthread_t thread1;

    // SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    int running = 1;
    // char serverIP[16];
    IPaddress ip;
    // if (SDLNet_ResolveHost(&ip, "localhost", 1234))
    //{
    //    SDLNet_ResolveHost(&ip, "localhost", 1234);
    //}
    // else
    //{
    // do {
    // printf("Host Address: ");
    // scanf(" %15s", serverIP);
    //} while(SDLNet_ResolveHost(&ip, serverIP, 1234) == -1);
    //}
    if(SDLNet_ResolveHost(&ip, serverIP, 1234) == -1)
        return 1;

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(1);

    TCPsocket client = SDLNet_TCP_Open(&ip);

    SDLNet_TCP_AddSocket(sockets, client);

    // TCPsocket server = SDLNet_AllocSocketSet(1);
    // SDLNet_AddSocket(server, client)
    pthread_create(&thread1, NULL, readMessage, (void *)client);
    while (running)
    {
        while (SDLNet_CheckSockets(sockets, 0) > 0)
        {
            if (SDLNet_SocketReady(client))
            {
                char text[100];
                SDLNet_TCP_Recv(client, text, 99);
                printf("MESSAGE: %s\n", text);
                if (text[0] == '2')
                    running = 0;
            }
        }
    }
    pthread_join(thread1, NULL);

    // strcpy(message, "2");
    // SDLNet_TCP_Send(client, message, strlen(message)+1);
    // SDL_Delay(1);
    SDLNet_TCP_Close(client);

    SDLNet_Quit();
    SDL_Quit;
}

void *readMessage(void *client)
{
    // TCPsocket client1 = ((TCPsocket *)client);
    while (1)
    {
        // printf(": ");
        char message[1400];
        scanf("%s", message);
        if (message[0] == 'L' && message[1] == 'V' && message[2] == 0)
        {
            SDLNet_TCP_Send(((TCPsocket *)client), "2", 2);
            return NULL;
        }
        else
            SDLNet_TCP_Send(((TCPsocket *)client), message, strlen(message) + 1);
    }

    return NULL;
}