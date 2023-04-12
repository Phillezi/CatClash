#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <SDL2/SDL.h>

// DEFINITIONS
#define MAP_FILEPATH "resources/map.txt"
#define SAVE_MAP_PATH "resources/"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 1024
#define WINDOW_NAME "Test"
#define FPS 165
#define MAPSIZE 32
#define TILESIZE 16
#define TILES 4
#define MAX_NAME_LEN 21

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
struct player
{
    int id;                      // The id of the player (Multiplayer purposes)
    char name[MAX_NAME_LEN + 1]; // The name of the player (Multiplayer purposes)
    int hp;                      // Health-points
    int x, y;                    // Position in game
    SDL_Rect rect;               // Screen position and size
};
typedef struct player Player;

#endif