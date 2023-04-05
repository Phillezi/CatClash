#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PLAYERS 5

typedef struct data {
   int x;
   int y;
   int status;
} Data;

typedef struct playerInfo {
    Uint32 ip;
    Uint32 port;
    int id;
} Player;

/* \returns ID of new player, -1 on already connected player */
int newAddress(Player player[], UDPpacket *pRecieve);

int main(int argc, char **argv) {
    UDPsocket socketDesc;   // Socket descriptor
    UDPpacket *pRecieve;    // Pointer to packet memory
    UDPpacket *pSent;       
    
    int quit = 0, players = 0, id, a, b;
    Player player[MAX_PLAYERS] = {0, 0};

    // Error checking and initiliaziation

        // Initialize SDL_net
        if (SDLNet_Init() < 0) {
            fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
            exit(EXIT_FAILURE);
        }

        // Open socket
        if (!(socketDesc = SDLNet_UDP_Open(2000))) {
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
            if (players < MAX_PLAYERS){
                if (players = newAddress(player, pRecieve)) {
                    player[id].ip = pRecieve->address.host;
                    player[id].port = pRecieve->address.port;
                    player[id].id = players;
                    printf("Client %d has connected!\n", players);
                }
            }

            // Find packet sender among players
            for (int i = 0; i < players; i++)
                if (pRecieve->address.port == player[i].port) { id = player[i].id; printf("Recieved data from player %d", id); }

            // Mirror packet to other players
            for (int i = 0; i < players; i++){
                if (id != player[i].id) {  
                    printf("Send to client %d", player[i].id);

                    pSent->address.host = player[i].ip;     // Set destination host
                    pSent->address.port = player[i].port;     // Set destination port

                    // Print data on server as well
                    sscanf((char * )pRecieve->data, "%d %d\n", &a, &b);
                    printf("%d %d\n", a, b);

                    sprintf((char *)pSent->data, "%d %d\n", a,  b);
                    pSent->len = strlen((char *)pSent->data) + 1;
                    SDLNet_UDP_Send(socketDesc, -1, pSent);
                }
            }
            
            // Quit main loop if packet contains "quit"
            if (strcmp((char *)pSent->data, "quit") == 0) quit = 1;
        }
    }


    /* Clean and exit */
	SDLNet_FreePacket(pSent);
    SDLNet_FreePacket(pRecieve);
	SDLNet_Quit();
	return EXIT_SUCCESS;
}

int newAddress(Player player[], UDPpacket *pRecieve) {
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (pRecieve->address.port == player[i].port) return -1;
        else if (player[i].ip == 0) return i; 
    }
}