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

    memcpy(&udpData, (char * ) pGame->pPacket->data, sizeof(Player));
    memcpy(&pGame->player, (char * ) pGame->pPacket->data, sizeof(Player));    // OBS! Can't handle list atm as we lack player array
    printf("UDP Packet incoming \tid: %d\tx: %d\ty: %d\n", udpData.id, udpData.x, udpData.y);
}