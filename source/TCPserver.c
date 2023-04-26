#include "definitions.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>
#include "init.h"

#define MAX_SOCKETS 5
#define PORT 1234
#define MAX_PACKET_SIZE 1024

int UDPserverTwo();

int main(int argc, char **argv)
{
    SDLNet_Init();
    Tile map[1024];
    char mapName[100];
    do
    {
        printf("Which map do you want to host?\n:");
        scanf(" %100s", mapName);
    } while (initMap(map, mapName, 16));

    int playerCount = 0, playerId = 0;

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, PORT);

    SDLNet_SocketSet sockets = SDLNet_AllocSocketSet(MAX_SOCKETS);

    TCPsocket server = SDLNet_TCP_Open(&ip);

    printf("Server is up and running\n");
    printf("Server is hosted at: %s:%d\n", SDLNet_ResolveIP(&ip), PORT);

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
                for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                    SDLNet_TCP_Send(tmpClient, &map[i].type, sizeof(map[i].type)); // send map to client
                SDLNet_TCP_Close(tmpClient);                                       // close client
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

    UDPserverTwo();

    SDL_Quit;
}

int UDPserverTwo()
{
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        return 1;
    }

    UDPsocket serverSocket = SDLNet_UDP_Open(1234);
    if (!serverSocket)
    {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        return 1;
    }

    UDPpacket *pPacket = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!pPacket)
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        return 1;
    }

    SDL_Window *pWindow = SDL_CreateWindow("UDP Server", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 320, 0);

    bool exit = false;
    while(!exit){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                exit = true;
                break;
            }
        }
        if(SDLNet_UDP_Recv(serverSocket, pPacket)){
            PacketData data;
            memcpy(&data, pPacket->data, sizeof(PacketData));
            printf("RECIVED: x:%d y:%d id:%d\n", data.x, data.y, data.id);
        }
    }

    SDL_DestroyWindow(pWindow);
    SDLNet_FreePacket(pPacket);
    SDLNet_UDP_Close(serverSocket);
    SDLNet_Quit();
    return 0;

}
/*
int UDPserver()
{
    UDPsocket server_socket;
    IPaddress server_address;
    UDPpacket *packet;
    int quit = 0;

    // Initialize SDL_net
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Create a UDP socket
    server_socket = SDLNet_UDP_Open(PORT);
    if (!server_socket)
    {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Allocate a packet buffer
    packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!packet)
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Main server loop
    while (!quit)
    {
        // Wait for a packet to arrive
        if (SDLNet_UDP_Recv(server_socket, packet))
        {
            // Extract the packet data
            PacketData data;
            memcpy(&data, packet->data, sizeof(PacketData));
            printf("RECIVED: x:%d y:%d id:%d\n", data.x, data.y, data.id);

            // Forward the packet to all other connected users
            int i;
            for (i = 0; i < SDLNet_UDP_GetNumSockets(server_socket); i++)
            {
                if (server_socket != SDLNet_UDP_GetSocket(server_socket, i))
                {
                    // Copy the packet data to a new packet buffer
                    UDPpacket *new_packet = SDLNet_AllocPacket(sizeof(PacketData));
                    memcpy(new_packet->data, &data, sizeof(PacketData));
                    new_packet->len = sizeof(PacketData);
                    new_packet->address = *SDLNet_UDP_GetPeerAddress(server_socket, i);
                    new_packet->channel = -1;

                    // Send the packet
                    if (SDLNet_UDP_Send(server_socket, -1, new_packet) < 0)
                    {
                        printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
                    }

                    // Free the packet buffer
                    SDLNet_FreePacket(new_packet);
                }
            }
        }
    }

    // Clean up and exit
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(server_socket);
    SDLNet_Quit();
    return 0;
}
*/

// new
/*
int UDPserverTwo()
{
    // Initialize SDL
    if (SDL_Init(0) != 0)
    {
        printf("SDL_Init: %s\n", SDL_GetError());
        exit(1);
    }

    // Initialize SDL_net
    if (SDLNet_Init() < 0)
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Create a socket set to manage clients
    SDLNet_SocketSet client_set = SDLNet_AllocSocketSet(MAX_SOCKETS);
    if (!client_set)
    {
        printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Create a UDP socket to listen for incoming packets
    UDPsocket server_socket = SDLNet_UDP_Open(PORT);
    if (!server_socket)
    {
        printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Add the server socket to the socket set
    if (SDLNet_UDP_AddSocket(client_set, server_socket) == -1)
    {
        printf("SDLNet_UDP_AddSocket: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Allocate a buffer for incoming packets
    UDPpacket *packet = SDLNet_AllocPacket(sizeof(PacketData));
    if (!packet)
    {
        printf("SDLNet_AllocPacket: %s\n", SDLNet_GetError());
        exit(1);
    }

    // Array to keep track of connected clients
    UDPsocket client_sockets[MAX_SOCKETS];
    bool client_connected[MAX_SOCKETS] = {false};
    int num_clients = 0;

    // Listen for incoming packets
    while (true)
    {
        int num_ready = SDLNet_CheckSockets(client_set, -1);
        if (num_ready == -1)
        {
            printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
            exit(1);
        }

        if (num_ready > 0)
        {
            if (SDLNet_SocketReady(server_socket))
            {
                int recv_bytes = SDLNet_UDP_Recv(server_socket, packet);
                if (recv_bytes > 0)
                {
                    // Get the packet data
                    PacketData data;
                    memcpy(&data, packet->data, sizeof(PacketData));
                    printf("RECIVED: x:%d y:%d id:%d\n", data.x, data.y, data.id);

                    // Forward the packet to all connected clients
                    for (int i = 0; i < num_clients; i++)
                    {
                        if (client_connected[i])
                        {
                            if (SDLNet_UDP_Send(client_sockets[i], -1, packet) < 0)
                            {
                                printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
                            }
                        }
                    }

                    // Check if the client is already connected
                    bool client_already_connected = false;
                    for (int i = 0; i < num_clients; i++)
                    {
                        if (client_sockets[i]->address.host == packet->address.host && client_sockets[i]->address.port == packet->address.port)
                        {
                            client_already_connected = true;
                            break;
                        }
                    }
                    // Add the client to the array if not already connected
                    if (!client_already_connected)
                    {
                        if (num_clients < MAX_SOCKETS)
                        {
                            client_sockets[num_clients] = SDLNet_UDP_Open(0);
                            if (!client_sockets[num_clients])
                            {
                                printf("SDLNet_UDP_Open: %s\n", SDLNet_GetError());
                                exit(1);
                            }

                            if (SDLNet_UDP_Bind(client_sockets[num_clients], -1, &packet->address) == -1)
                            {
                                printf("SDLNet_UDP_Bind: %s\n", SDLNet_GetError());
                                exit(1);
                            }

                            if (SDLNet_UDP_AddSocket(client_set, client_sockets[num_clients]) == -1)
                            {
                                printf("SDLNet_UDP_AddSocket: %s\n", SDLNet_GetError());
                                exit(1);
                            }

                            client_connected[num_clients] = true;
                            num_clients++;
                        }
                    }
                }
            }

            // Check for packets from connected clients
            for (int i = 0; i < num_clients; i++)
            {
                if (client_connected[i] && SDLNet_SocketReady(client_sockets[i]))
                {
                    int recv_bytes = SDLNet_UDP_Recv(client_sockets[i], packet);
                    if (recv_bytes > 0)
                    {
                        // Forward the packet to all other connected clients
                        for (int j = 0; j < num_clients; j++)
                        {
                            if (j != i && client_connected[j])
                            {
                                if (SDLNet_UDP_Send(client_sockets[j], -1, packet) < 0)
                                {
                                    printf("SDLNet_UDP_Send: %s\n", SDLNet_GetError());
                                }
                            }
                        }
                    }
                    else
                    {
                        // Client disconnected
                        SDLNet_UDP_DelSocket(client_set, client_sockets[i]);
                        SDLNet_UDP_Close(client_sockets[i]);
                        client_connected[i] = false;
                    }
                }
            }
        }
    }

    // Free resources
    SDLNet_FreePacket(packet);
    SDLNet_FreeSocketSet(client_set);
    SDLNet_UDP_Close(server_socket);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
*/
/*
void UDPserverTwo(UDPsocket server_socket) {
    UDPpacket* packet;
    UDPsocket* client_sockets[MAX_SOCKETS];
    int num_clients = 0;

    while (1) {
        // Receive packet
        packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
        if (SDLNet_UDP_Recv(server_socket, packet)) {
            printf("Received packet from %s:%d\n", SDLNet_ResolveIP(&packet->address), packet->address.port);

            // Check if client is already connected
            int i;
            for (i = 0; i < num_clients; i++) {
                if (client_sockets[i] != NULL && client_sockets[i]->address.host == packet->address.host && client_sockets[i]->address.port == packet->address.port) {
                    break;
                }
            }

            if (i == num_clients && num_clients < MAX_SOCKETS) {
                // New client, add to list
                client_sockets[num_clients] = SDLNet_UDP_Open(0);
                SDLNet_UDP_Bind(client_sockets[num_clients], -1, &packet->address);
                num_clients++;
                printf("New client connected\n");
            }

            // Forward packet to all other clients
            int j;
            for (j = 0; j < num_clients; j++) {
                if (client_sockets[j] != NULL && client_sockets[j] != packet->address.host) {
                    SDLNet_UDP_Send(client_sockets[j], -1, packet);
                }
            }
        }

        SDLNet_FreePacket(packet);
    }
}
*/