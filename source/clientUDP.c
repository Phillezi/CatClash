#include "clientUDP.h"
#include "player.h"

void sendData(Game *pGame)
{
    PlayerUdpPkg pkg;
    pkg.id = pGame->pPlayer->id;
    pkg.idle = pGame->pPlayer->idle;
    pkg.x = pGame->pPlayer->x;
    pkg.y = pGame->pPlayer->y;
    pkg.direction = pGame->pPlayer->prevKeyPressed;
    memcpy(pGame->pPacket->data, &pkg, sizeof(PlayerUdpPkg));

    pGame->pPacket->len = sizeof(PlayerUdpPkg);

    pGame->pPacket->address.host = pGame->serverAddress.host; // Set the destination host
    pGame->pPacket->address.port = pGame->serverAddress.port; // And destination port

    if (!SDLNet_UDP_Send(pGame->socketDesc, -1, pGame->pPacket))
    {
        printf("Could not send package\n");
    }
}
/*
void retrieveData(Game *pGame)
{
    Player udpData;

    if (strstr((char *)pGame->pPacket->data, "exit"))
    {
        if ((char *)pGame->pPacket->data[0] > 48 && (char *)pGame->pPacket->data[0] <= 53)
        {   // 48 = 0 and 53 = 5 in ASCII
            // pGame->nrOfPlayers--;
            /* for (size_t i = (int *)pGame->pPacket->data-1; i < MAX_PLAYERS; i++)  {
                pGame->players[i] = pGame->players[i+1];
            }
            */
//           printf("Hello!");
//        }
//    }
//    else
//    {
//        memcpy(&udpData, (char *)pGame->pPacket->data, sizeof(Player));
// memcpy(&pGame->players[udpData.id-1], (char * ) pGame->pPacket->data, sizeof(Player));    // OBS! Can't handle list atm as we lack player array
//        printf("UDP Packet incoming \tid: %d\tx: %d\ty: %d\n", udpData.id, udpData.x, udpData.y);
//        // if (pGame->nrOfPlayers+1 == udpData.id && pGame->nrOfPlayers < MAX_PLAYERS) pGame->nrOfPlayers++;     // Increments nrOfPlayers if new id is one higher than current nrOfPlayers
//    }
//}

int getPlayerData(Game *pGame, Player players[])
{
    for (int i = 0; i < MAX_PLAYERS; i++)
    {
        if (SDLNet_UDP_Recv(pGame->socketDesc, pGame->pPacket))
        {
            PlayerUdpPkg tmp;
            memcpy(&tmp, pGame->pPacket->data, sizeof(PlayerUdpPkg));
            if (tmp.id > pGame->nrOfPlayers)
            {
                pGame->pMultiPlayer = createNewMultiPlayer(pGame, pGame->nrOfPlayers, tmp);
                pGame->nrOfPlayers++;
                printf("A new player joined! (%d total)\n", pGame->nrOfPlayers );
                /*
                    Allocate memory for new player using Realloc
                */
            }
            else
            {
                for (int j = 0; j < pGame->nrOfPlayers; j++)
                {
                    if (pGame->pMultiPlayer[j].id == tmp.id)
                    {
                        printf("Recived msg from %d\n", tmp.id);
                        pGame->pMultiPlayer[j].x == tmp.x;
                        pGame->pMultiPlayer[j].y == tmp.y;
                        pGame->pMultiPlayer[j].idle == tmp.idle;
                    }
                }
            }
            /*
            if (!pGame->pPlayer->id)
            {
                if (tmp.id - 1 < MAX_PLAYERS)
                {
                    players[tmp.id - 1].x = tmp.x;
                    players[tmp.id - 1].y = tmp.y;
                    players[tmp.id - 1].id = tmp.id;
                    players[tmp.id - 1].idle = tmp.idle;
                    players[tmp.id - 1].prevKeyPressed = tmp.direction;
                }
            }
            else if (tmp.id - 1 < MAX_PLAYERS)
            {
                players[tmp.id - 1].x = tmp.x;
                players[tmp.id - 1].y = tmp.y;
                players[tmp.id - 1].id = tmp.id;
                players[tmp.id - 1].idle = tmp.idle;
                players[tmp.id - 1].prevKeyPressed = tmp.direction;
            }
            */
            // printf("Recived package\n");
        }
    }

    return 0;
}