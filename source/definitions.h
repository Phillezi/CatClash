#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>

// DEFINITIONS
#define MAP_FILEPATH "resources/map.txt"
#define SAVE_MAP_PATH "resources/maps/"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 1024
#define WINDOW_NAME "Test"
#define FPS 165
#define MAPSIZE 32
#define TILESIZE 16
#define TILES 4
#define MAX_NAME_LEN 21
#define MAX_CHARGE 100
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define MAX_PLAYERS 5

// ADTS

struct tile
{
    int x, y;
    SDL_Rect wall;
    int type;
};
typedef struct tile Tile;

/*
    Player structure:
    includes: int id, str name (max 20 chars + NULL),
    int hp, int x, y, SDL_Rect rect
*/
enum playerState{ALIVE, DEAD};
typedef enum playerState PlayerState;

struct player
{
    char prevKeyPressed;
    int idle;
    int charge;
    int id;                      // The id of the player (Multiplayer purposes)
    char name[MAX_NAME_LEN + 1]; // The name of the player (Multiplayer purposes)
    int hp;                      // Health-points
    PlayerState state;           // State of the player (alive or dead)
    int x, y;                    // Position in game
    SDL_Rect rect;               // Screen position and size
};
typedef struct player Player;

struct world
{
    int tileSize;
    float angle;
};
typedef struct world World;

struct config
{
    bool vSync;
    int fps;
    bool multiThreading;
};
typedef struct config Config;

struct text
{
    SDL_Rect rect;
    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;
};
typedef struct text Text;

struct uiElements
{
    SDL_Rect chargebar;
    SDL_Rect healthbar;
    SDL_Rect fpsFrame;
    Text *pMenuText, *pOverText, *pFpsText;
    TTF_Font *pGameFont, *pFpsFont;
};
typedef struct uiElements UiE;

enum gameState
{
    START,
    ONGOING,
    OVER
};
typedef enum gameState GameState;

typedef struct playerNet
{
    UDPsocket socketUDP;
    TCPsocket socketTCP;
    char serverIP[16];
    int port;
    int id;
    int x, y;
} PlayerNet;

struct game
{
    UiE ui;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;

    SDL_Texture *pTileTextures[TILES];
    SDL_Texture *pPlayerTexture;
    SDL_Rect gSpriteClips[27];

    int windowWidth, windowHeight;
    int movementAmount;

    UDPsocket socketDesc;
    IPaddress serverAddress;
    UDPpacket *pPacket;

    PlayerNet *pClient;

    //    Player player;
    Player players[MAX_PLAYERS];
    Player *pPlayer;
    Tile map[MAPSIZE * MAPSIZE];
    World world;
    GameState state;

    Config config;
};
typedef struct game Game;

// ------------------------------ SERVER ------------------------------------

enum serverState{JOINING, RUNNING, CLOSED};
typedef enum serverState ServerState;

struct info {
    IPaddress address;
    int id;
}; 
typedef struct info Info;

struct server {
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    TTF_Font *pFont;
    Text *pSpace, *pClosed, *pJoining, *pRunning, *pIP;

    Text *pClientText[MAX_PLAYERS];

    ServerState state;
    int windowWidth;
    int windowHeight;
    int fontSize;

    int nrOfClients;
    Info clients[MAX_PLAYERS];
    Player playerData[MAX_PLAYERS];

    TCPsocket socketTCP;
    IPaddress TCPip;
    SDLNet_SocketSet socketSetTCP;
    Tile map[1024];

    UDPsocket socketUDP;   // Socket descriptor
    UDPpacket *pRecieve;    // Pointer to packet memory
    UDPpacket *pSent;           
}; 
typedef struct server Server;

struct udpPlayerPackage{
    int x,y;
    char direction;
    Uint8 id;
    Uint8 idle;
};
typedef struct udpPlayerPackage PlayerUdpPkg;

#endif
