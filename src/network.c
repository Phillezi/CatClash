#include "network.h"

Network *createNetwork()
{
    Network *pNew_network = (Network *)malloc(sizeof(Network));
    if (!pNew_network)
    {
        printf("Failed to allocate memory for network\n");
        return NULL;
    }
    if (initalizenetwork(pNew_network))
    {
        printf("Failed to initialize network\n");
        return NULL;
    }
    return pNew_network;
}

void destroyNetwork(Network *pNetwork)
{
    if(pNetwork->pPacket)
        SDLNet_FreePacket(pNetwork->pPacket);

    SDLNet_Quit();
    
    if(pNetwork)
        free(pNetwork);
}

int initalizenetwork(Network *pNew_network)
{
    if(SDLNet_Init())
    {
        printf("ERROR: %s\n", SDLNet_GetError());
        return 1;
    }

    pNew_network->pPacket = SDLNet_AllocPacket(sizeof(Player));

    return 0;
}