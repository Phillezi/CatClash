#ifndef CLIENTUDP_H
#define CLIENTUDP_H

#include <SDL2/SDL_net.h>
#include "definitions.h"

void sendData(Player udpData, UDPpacket *p, IPaddress srvadd, UDPsocket sd);
void retrieveData(Player players[], UDPpacket *p);

#endif