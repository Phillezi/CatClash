#ifndef CLIENTUDP_H
#define CLIENTUDP_H

#include <SDL2/SDL_net.h>
#include "definitions.h"

void sendData(Player udpData, UDPpacket *p, IPaddress srvadd, UDPsocket sd);
Player retrieveData(Player currentPlayers[], UDPpacket *p);

#endif