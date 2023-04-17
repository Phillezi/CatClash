#include "clientUDP.h"

void sendData(Player udpData, UDPpacket *p, IPaddress srvadd, UDPsocket sd);
Player retrieveData(Player currentPlayers[], UDPpacket *p);

void sendData(Player udpData, UDPpacket *p, IPaddress srvadd, UDPsocket sd) {
    memcpy(p->data, &udpData, sizeof(Player)+1);

    p->len = sizeof(Player)+1;

    p->address.host = srvadd.host;	/* Set the destination host */
    p->address.port = srvadd.port;	/* And destination port */

    SDLNet_UDP_Send(sd, -1, p);
}

Player retrieveData(Player currentPlayers[], UDPpacket *p) {
    Player udpData;

    memcpy(&udpData, (char * ) p->data, sizeof(Player));
    memcpy(&currentPlayers[udpData.id-1], (char * ) p->data, sizeof(Player));
    printf("UDP Packet incoming \tid: %d\tx: %d\ty: %d\n", udpData.id, udpData.x, udpData.y);

    return udpData;
}