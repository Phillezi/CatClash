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
    if(pNetwork)
        free(pNetwork);
}

int initalizenetwork(Network *pNew_network)
{

}