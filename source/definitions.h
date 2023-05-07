#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// DEFINITIONS
#define MAP_FILEPATH "resources/map.txt"
#define SAVE_MAP_PATH "resources/maps/"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 1024
#define WINDOW_NAME "Test"
#define FPS 165
#define MAPSIZE 32
#define TILESIZE 16
#define TILES 20
#define MAX_NAME_LEN 21
#define MAX_CHARGE 100
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define MAX_PLAYERS 5
#define HEIGHT_OF_PLAYER_SPRITE 544

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
enum playerState
{
    ALIVE,
    DEAD,
    WIN
};
typedef enum playerState PlayerState;

enum selectedMode
{
    PLAY,
    EDIT,
    JOIN,
    HOST,
    CATSEL,
    QUIT
};
typedef enum selectedMode SelectedMode;

struct player
{
    char prevKeyPressed;
    Uint8 idle;
    int charge;
    Uint8 id;                      // The id of the player (Multiplayer purposes)
    Uint8 charging;
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
    Text *pMenuText, *pOverText, *pFpsText, *pPlayerName, *pWinText;
    TTF_Font *pGameFont, *pFpsFont, *pNameTagFont;
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
    SDLNet_SocketSet sockets;
    char serverIP[16];
    int port;
    int id;
    int x, y;
} PlayerNet;

struct game
{
    bool isConnected;
    bool isDrawing;
    int tempID;
    int nrOfPlayers;
    int nrOfPlayersAlive;
    bool serverIsHosted;
    pthread_t serverThread;
    Player *pMultiPlayer;
    UiE ui;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;

    SDL_Texture *pTileTextures[TILES];
    SDL_Texture *pPlayerTexture;
    SDL_Rect gSpriteClips[MAX_PLAYERS][29];

    SDL_Rect portalList[MAPSIZE * MAPSIZE];
    int nrOfPortals;

    int windowWidth, windowHeight;
    int movementAmount;

    UDPsocket socketDesc;
    IPaddress serverAddress;
    UDPpacket *pPacket;

    PlayerNet *pClient;

    //    Player player;
    Player *pPlayer;
    Tile map[MAPSIZE * MAPSIZE];
    World world;
    GameState state;

    Config config;
};
typedef struct game Game;

// ------------------------------ SERVER ------------------------------------

enum serverState
{
    JOINING,
    RUNNING,
    CLOSED
};
typedef enum serverState ServerState;

enum STCPSTATE{CLIENT_JOINING, SENDING_MAP, SENDING_PLAYER_ID, GET_PLAYER_DATA, SEND_NEW_PLATER_DATA, IDLE};
typedef enum STCPSTATE TcpState;

struct info
{
    IPaddress address;
    int id;
    TCPsocket tcpSocket;
    Player data;
    Uint32 timeout;
};
typedef struct info Info;

struct server
{
    int mapPos;
    TcpState tcpState;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    TTF_Font *pFont;
    Text *pSpace, *pClosed, *pJoining, *pRunning, *pIP;
    Text *pServerStateText;
    SDL_Rect progressBar;

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

    UDPsocket socketUDP; // Socket descriptor
    UDPpacket *pRecieve; // Pointer to packet memory
    UDPpacket *pSent;
};
typedef struct server Server;

struct udpPlayerPackage
{
    int x, y;
    char direction;
    Uint8 id;
    Uint8 idle;
    Uint8 charge;
<<<<<<< Updated upstream
=======
    Uint8 charging;
    Uint8 leavingFlag;
    PlayerState state;
>>>>>>> Stashed changes
    Uint8 charging;
    PlayerState state;
};
typedef struct udpPlayerPackage PlayerUdpPkg;

#endif
