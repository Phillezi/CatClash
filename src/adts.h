#ifndef ADTS_H
#define ADTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_TTF.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define TILES 19
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define MAPSIZE 32


/* ADT for Text*/
typedef struct {
    SDL_Rect rect;
    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;
} Text;

/* ADT for Window*/
typedef struct {
    Uint32 width, height;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    Uint32 frameCounter;
    Uint32 fps;
    
} Window;

/* Enum for menu selection*/
typedef enum {
    MENU_PLAY,
    MENU_EDIT,
    MENU_JOIN,
    MENU_QUIT
} Selection;

/* ADT for Menu*/
typedef struct {
    Selection selection;
    Uint32 fontSize;
    TTF_Font *pfont;
    Text *pPlayText;
    Text *pEditText;
    Text *pJoinText;
    Text *pQuitText;
} Menu;

/* ADT for Player*/
typedef struct {
    int x;
    int y;
    SDL_Rect rect;

} Player;

/* ADT for Tile*/
typedef struct {
    int x;
    int y;
    int type;
    SDL_Rect rect;

} Tile;

/* ADT for Game*/
typedef struct {
    Player *pPlayers;
    Tile *pMap;
    SDL_Texture **ppTileTextures;
    int tileSize;
    int movementAmount;
} Game;

/* ADT for Network*/
typedef struct {
    // TCP
    TCPsocket socketTCP;
    SDLNet_SocketSet sockets;

    // UDP
    UDPsocket socketUDP;
    UDPpacket *pPacket;
    
} Network;

/* Enum for application states*/
typedef enum {
    PLAY,
    SELECT_MAP,
    EDIT,
    JOIN,
    SELECT_SERVER,
    QUIT,
    MENU
} State;

/* Struct for Semaphores*/
typedef struct {
    sem_t updateWindow;
    sem_t updateInput;
    sem_t updateNetwork;
} Semaphores;

/* ADT for Application*/
typedef struct {
    Window *pWindow;
    Network *pNetwork;
    Game *pGame;
    bool exit;
    State state;
    Semaphores semaphore;
    Menu *pMenu;
} App;

#endif