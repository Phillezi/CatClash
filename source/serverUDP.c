#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "definitions.h"

#define MAX_PLAYERS 5
#define PORT 1234

typedef struct playerInfo {
    Uint32 ip;
    Uint32 port;
    int id;
} PlayerInfo;

/* \returns ID of new player, -1 on already connected player */
int newAddress(PlayerInfo player[], UDPpacket *pRecieve);

int main(int argc, char **argv) {
    UDPsocket socketDesc;   // Socket descriptor
    UDPpacket *pRecieve;    // Pointer to packet memory
    UDPpacket *pSent;       
    
    int quit = 0, playerCount = 0, id;
    char a[20];
    PlayerInfo player[MAX_PLAYERS] = {0,0,0};
    Player udpData;

    // Error checking and initiliaziation

        // Initialize SDL_net
        if (SDLNet_Init() < 0) {
            fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }

        // Open socket
        if (!(socketDesc = SDLNet_UDP_Open(PORT))) {
            fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }

        // Make space for future packets
        if (!((pSent = SDLNet_AllocPacket(512)) && (pRecieve = SDLNet_AllocPacket(512)))) {
            fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE); 
        }
    
    // Main loop
    while (!quit) {
        
        // Get ready to recieve packet - UDP_Recv != 0 if a packet is coming
        if (SDLNet_UDP_Recv(socketDesc, pRecieve)) {

            // Add new players
            if (playerCount < MAX_PLAYERS){
                if (newAddress(player, pRecieve) > 0) {
                    playerCount++;
                    player[playerCount - 1].ip = pRecieve->address.host;
                    player[playerCount - 1].port = pRecieve->address.port;
                    player[playerCount - 1].id = playerCount;
                    printf("Client %d has connected!\n", playerCount);
                }
            }

            // Find packet sender among players
            for (int i = 0; i < playerCount; i++)
                if (pRecieve->address.port == player[i].port) { id = player[i].id; printf("Recieved data from player %d\n", id); }

            // Remove player if packet contains "exit" and adjust other players
            if (!strcmp((char *)pRecieve->data, "exit")) {
                printf("Player %d has exited\n", id);

                sprintf((char *)pSent->data, "%c exit", id+48); // 48 = 0 in ASCII 
                pSent->len = strlen((char *)pSent->data) + 1;
                SDLNet_UDP_Send(socketDesc, -1, pSent);

                for (int i = id-1; i < playerCount-1; i++) {
                    player[i] = player[i+1];
                    player[i].id--;
                    printf("- Player %d is now player %d\n", i+2, player[i].id);
                }
                playerCount--;
            } else {    // Mirror packet to other players
                for (int i = 0; i < playerCount; i++){
                    if (id != player[i].id) {  
                        printf("Send to client %d\n", player[i].id);

                        pSent->address.host = player[i].ip;       // Set destination host
                        pSent->address.port = player[i].port;     // Set destination port

                        //sscanf((char * )pRecieve->data, "%s\n", &a);
                        //printf("%s\n", a);

                        // Copy recieved data to another package and print for error handling
                        memcpy(&udpData, (char *)pRecieve->data, sizeof(Player));
                        printf("UDP Packet data: %d %d", udpData.x, udpData.y);
                        memcpy((char *)pSent->data, &udpData, sizeof(Player)+1);

                        //sprintf((char *)pSent->data, "%s", a);
                        pSent->len = sizeof(Player) + 1;
                        SDLNet_UDP_Send(socketDesc, -1, pSent);
                    }
                }
            }

            // Quit main loop if packet contains "quit"
            if (!strcmp((char *)pRecieve->data, "quit")) quit = 1;
        }
    }


    /* Clean and exit */
	SDLNet_FreePacket(pSent);
    SDLNet_FreePacket(pRecieve);
	SDLNet_Quit();
	return EXIT_SUCCESS;
}

int newAddress(PlayerInfo player[], UDPpacket *pRecieve) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pRecieve->address.port == player[i].port)   // known user?
            return -1;                                  // yes return -1
        else if (player[i].ip == 0)                     // not known user, empty ip att player[i].ip?
            return (i+1);                               // yes return id of new player;
    }
}