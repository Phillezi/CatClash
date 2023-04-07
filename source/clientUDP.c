#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2\SDL_net.h>

int main(int argc, char **argv)
{
	UDPsocket socketDesc;
	IPaddress srvadd;
	UDPpacket *pSent;
	int quit;
 
	/* Check for parameters */
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s host port\n", argv[0]);
		exit(EXIT_FAILURE);
	}
 
	/* Initialize SDL_net */
	if (SDLNet_Init() < 0)
	{
		fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Open a socket on random port */
	if (!(socketDesc = SDLNet_UDP_Open(0)))
	{
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Resolve server name  */
	if (SDLNet_ResolveHost(&srvadd, argv[1], atoi(argv[2])) == -1)
	{
		fprintf(stderr, "SDLNet_ResolveHost(%s %d): %s\n", argv[1], atoi(argv[2]), SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Allocate memory for the packet */
	if (!(pSent = SDLNet_AllocPacket(512)))
	{
		fprintf(stderr, "SDLNet_AllocPacket: %s\n", SDLNet_GetError());
		exit(EXIT_FAILURE);
	}
 
	/* Main loop */
	quit = 0;
	while (!quit)
	{
		printf("Fill the buffer\n>");
		scanf("%s", (char *)pSent->data);
 
		pSent->address.host = srvadd.host;	/* Set the destination host */
		pSent->address.port = srvadd.port;	/* And destination port */
 
		pSent->len = strlen((char *)pSent->data) + 1;
		SDLNet_UDP_Send(socketDesc, -1, pSent); /* This sets the pSent->channel */
 
		/* Quit if packet contains "quit" */
		if (!strcmp((char *)pSent->data, "quit"))
			quit = 1;
	}
 
	SDLNet_FreePacket(pSent);
	SDLNet_Quit();
 
	return EXIT_SUCCESS;
} 