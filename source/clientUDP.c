#include "clientUDP.h"

void sendData(Game *pGame) {
    memcpy(pGame->pPacket->data, &pGame->player, sizeof(Player)+1);

    pGame->pPacket->len = sizeof(Player)+1;

    pGame->pPacket->address.host = pGame->serverAddress.host;	// Set the destination host 
    pGame->pPacket->address.port = pGame->serverAddress.port;	// And destination port 

    SDLNet_UDP_Send(pGame->socketDesc, -1, pGame->pPacket);
}

void retrieveData(Game *pGame) {
    Player udpData;

    if (strstr((char *)pGame->pPacket->data, "exit")) {
        if ((char *)pGame->pPacket->data[0] > 48 && (char *)pGame->pPacket->data[0] <= 53) { // 48 = 0 and 53 = 5 in ASCII
            //pGame->nrOfPlayers--;
            /* for (size_t i = (int *)pGame->pPacket->data-1; i < MAX_PLAYERS; i++)  {
                pGame->players[i] = pGame->players[i+1];
            }
            */
           printf("Hello!");
        }
    }
    else {
        memcpy(&udpData, (char * ) pGame->pPacket->data, sizeof(Player));
        //memcpy(&pGame->players[udpData.id-1], (char * ) pGame->pPacket->data, sizeof(Player));    // OBS! Can't handle list atm as we lack player array
        printf("UDP Packet incoming \tid: %d\tx: %d\ty: %d\n", udpData.id, udpData.x, udpData.y);
        //if (pGame->nrOfPlayers+1 == udpData.id && pGame->nrOfPlayers < MAX_PLAYERS) pGame->nrOfPlayers++;     // Increments nrOfPlayers if new id is one higher than current nrOfPlayers
    }
}