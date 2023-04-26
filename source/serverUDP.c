#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "definitions.h"
#include "text.h"

#define MAX_PLAYERS 5
#define PORT 1234

void checkClient(Server *pServer, Player data);
int setupUDP(Server *pServer);
void runUDP(Server *pServer);
void closeUDP(Server *pServer);
// void addClient(Server *pServer);
void send(Server *pServer, Player udpData, int id);
int findID(Server *pServer);
int checkExit(Server *pServer, int id);
void recieveAndSend(Server *pServer, Player udpData);

void checkClient(Server *pServer, Player data)
{
    for (int i = 0; i < 5; i++)
    {
        if (pServer->clients[i].address.port == pServer->pRecieve->address.port && pServer->clients[i].address.host == pServer->pRecieve->address.host) // is this the sender?
        {
            // sender is known
            printf("Sender is known (%d)\n", i);
            return;
        }
        else // no
        {
            if ((pServer->clients[i].address.port == 8888) && (pServer->clients[i].address.host == 8888)) // is it a empty slot?
            {
                pServer->clients[i].address.port = pServer->pRecieve->address.port; // Save client in slot
                pServer->clients[i].address.host = pServer->pRecieve->address.host;
                pServer->clients[i].id = data.id;
                return;
            }
            else // not the sender nor empty slot
            {
                printf("Sender is not %d ( %d:%d )\n", i, pServer->clients[i].address.host, pServer->clients[i].address.port);
                /*
                memcpy(&pServer->pSent->data, pServer->pRecieve->data, sizeof(Player));
                pServer->pSent->address.port = pServer->clients[i].address.port;
                pServer->pSent->address.host = pServer->clients[i].address.host;
                pServer->pSent->len = pServer->pRecieve->len;

                SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pSent); // Send to them since they arnn't the sender nor empty
                */
            }
        }
    }
}

int serverUDP(Server *pServer)
{
    if (!setupUDP(pServer))
        return 1;
    runUDP(pServer);
    closeUDP(pServer);
}

int setupUDP(Server *pServer)
{
    // Open socket
    if (!(pServer->socketUDP = SDLNet_UDP_Open(PORT)))
    {
        fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }
    // Make space for future packets
    if (!((pServer->pSent = SDLNet_AllocPacket(512)) && (pServer->pRecieve = SDLNet_AllocPacket(512))))
    {
        fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; i++)
    {
        pServer->clients[i].address.port = 8888;
        pServer->clients[i].address.host = 8888;
        pServer->clients[i].id = 0;
    }
}

void runUDP(Server *pServer)
{
    int quit = 0;
    SDL_Event event;
    Player udpData;
    char prompt[12];

    while (!quit)
    {
        SDL_SetRenderDrawColor(pServer->pRenderer, 255, 255, 255, 255);
        SDL_RenderClear(pServer->pRenderer);

        sprintf(prompt, "Clients: %d", pServer->nrOfClients);
        Text *pPrompt = createText(pServer->pRenderer, 102, 205, 170, pServer->pFont, prompt, pServer->windowWidth / 2, 4 * (pServer->windowHeight / MAPSIZE) * 4);

        switch (pServer->state)
        {
        /*
        case CLOSED:
            drawText(pServer->pSpace, pServer->pRenderer);
            drawText(pServer->pClosed, pServer->pRenderer);
            if(SDL_PollEvent(&event)) {
                if (event.type==SDL_QUIT) quit = 1;
                if (event.type==SDL_KEYDOWN)
                    if (event.key.keysym.sym == SDLK_SPACE) pServer->state = JOINING;
            }
            break;
        case JOINING:
            drawText(pServer->pSpace, pServer->pRenderer);
            drawText(pServer->pJoining, pServer->pRenderer);
            drawText(pPrompt, pServer->pRenderer);

            if(SDL_PollEvent(&event)) {
                if (event.type==SDL_QUIT) quit = 1;
                if (event.type==SDL_KEYDOWN && pServer->nrOfClients >= 2)
                    if (event.key.keysym.sym == SDLK_SPACE) pServer->state = RUNNING;
            }
            if(pServer->state == RUNNING) send(pServer, udpData, -1);
            if(SDLNet_UDP_Recv(pServer->socketUDP, pServer->pRecieve)==1 && pServer->nrOfClients < MAX_PLAYERS)
                { addClient(pServer); checkExit(pServer, findID(pServer)); }
            break;
        */
        case JOINING:
            quit = 1;
            break;
        case RUNNING:
            drawText(pServer->pRunning, pServer->pRenderer);
            drawText(pPrompt, pServer->pRenderer);

            if (SDL_PollEvent(&event))
                if (event.type == SDL_QUIT)
                    quit = 1;
            if (SDLNet_UDP_Recv(pServer->socketUDP, pServer->pRecieve) == 1)
            {
                // printf("Recived msg\n");
                //  send(pServer, udpData, 0); // temp test
                // recieveAndSend(pServer, udpData);
                Player data;
                memcpy(&data, pServer->pRecieve->data, sizeof(Player));
                printf("Recived: id: %d, x: %d, y: %d from %d:%d\n", data.id, data.x, data.y, pServer->pRecieve->address.host, pServer->pRecieve->address.port);
                checkClient(pServer, data);
                for (int i = 0; i < pServer->nrOfClients; i++)
                {
                    if (pServer->clients[i].id != data.id)
                    {
                        memcpy(&pServer->pSent->data, pServer->pRecieve, sizeof(pServer->pRecieve));
                        pServer->pSent->address.port = pServer->clients[i].address.port;
                        pServer->pSent->address.host = pServer->clients[i].address.host;

                        if(!SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pSent)){
                            printf("Error: Could not send package\n");
                        }
                    }
                }

                // SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pRecieve);
            }
            break;
        }

        freeText(pPrompt);
        SDL_RenderPresent(pServer->pRenderer);
    }
}

void closeUDP(Server *pServer)
{
    if (pServer->pRecieve)
        SDLNet_FreePacket(pServer->pRecieve);
    if (pServer->pSent)
        SDLNet_FreePacket(pServer->pSent);
    if (pServer->socketUDP)
        SDLNet_UDP_Close(pServer->socketUDP);

    SDLNet_FreePacket(pServer->pSent);
    SDLNet_FreePacket(pServer->pRecieve);
}

/* // Compares port to connected players ports and if there are less than 5 players adds new players to a list
void addClient(Server *pServer) {
    for (int i = 0; i < MAX_PLAYERS; i++) if (pServer->pRecieve->address.port == pServer->clients[i].address.port) return;
    pServer->clients[pServer->nrOfClients].address = pServer->pRecieve->address;    // Adds client address
    pServer->clients[pServer->nrOfClients].id = pServer->nrOfClients + 1;           // Adds client id
    pServer->nrOfClients++;
}
*/

void send(Server *pServer, Player udpData, int id)
{
    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        if (id != pServer->clients[i].id)
        {
            printf("Send to client %d\n", pServer->clients[i].id);

            pServer->pSent->address.host = pServer->clients[i].address.host; // Set destination host
            pServer->pSent->address.port = pServer->clients[i].address.port; // Set destination port

            // Copy recieved data to another package and print for error handling
            memcpy(&udpData, (char *)pServer->pRecieve->data, sizeof(Player));
            udpData.id = pServer->clients[i].id;
            memcpy((char *)pServer->pSent->data, &udpData, sizeof(Player) + 1);

            // sprintf((char *)pSent->data, "%s", a);
            pServer->pSent->len = sizeof(Player) + 1;
            SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pSent);
        }
    }
}

int findID(Server *pServer)
{
    int id;

    // Find packet sender among players
    for (int i = 0; i < pServer->nrOfClients; i++)
    {
        if (pServer->pRecieve->address.port == pServer->clients[i].address.port)
        {
            id = pServer->clients[i].id;
            printf("Recieved data from player %d\n", id);
            return id;
        }
        else
        {
            printf("Setting address port %d to %d\n", i, pServer->clients[i].address.port);
            pServer->clients[i].id = pServer->pRecieve->address.port;
            id = pServer->clients[i].id;
            printf("Recieved data from player %d\n", id);
            return id;
        }
    }

    return 0;
}

int checkExit(Server *pServer, int id)
{
    if (!strcmp((char *)pServer->pRecieve->data, "exit"))
    {
        printf("Player %d has exited\n", id);

        sprintf((char *)pServer->pSent->data, "%c exit", id + 48); // 48 = 0 in ASCII
        pServer->pSent->len = strlen((char *)pServer->pSent->data) + 1;

        for (int i = id - 1; i < pServer->nrOfClients - 1; i++)
        {
            pServer->clients[i] = pServer->clients[i + 1];
            pServer->clients[i].id--;
            printf("- Player %d is now player %d\n", i + 2, pServer->clients[i].id);
        }
        pServer->nrOfClients--;

        for (int i = 0; i < pServer->nrOfClients; i++)
        {
            if (id != pServer->clients[i].id)
            {
                pServer->pSent->address.host = pServer->clients[i].address.host; // Set destination host
                pServer->pSent->address.port = pServer->clients[i].address.port; // Set destination port
                SDLNet_UDP_Send(pServer->socketUDP, -1, pServer->pSent);
            }
        }
        return 0;
    }
    else
        return 1;
}

/* Retrieves and resends data from UDP port. Rejects data from unknown players. */
void recieveAndSend(Server *pServer, Player udpData)
{
    // Find packet sender among players
    int id = findID(pServer);

    // If client is not known, return
    if (!id)
        return;

    if (!checkExit(pServer, id))
        return; // If not exit,
    else
        send(pServer, udpData, id); // Resend data to other players
}