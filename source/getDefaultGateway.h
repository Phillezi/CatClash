#ifndef GETDEFAULTGATEWAY_H
#define GETDEFAULTGATEWAY_H

/*
    getDefaultGateway
    This function gets the default gateway of the network
    expected in param: char gatewayIp[INET_ADDRSTRLEN]
    INET_ADDRSTRLEN is usually 16
    returns 0 on succes and 1 on failure
*/
int getDefaultGateway(char *gateway_ip);

#endif