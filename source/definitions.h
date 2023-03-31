#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <SDL2/SDL.h>

// DEFINITIONS
#define MAP_FILEPATH "resources/map.txt"
#define SAVE_MAP_FILEPATH "resources/savedMap.txt"
#define DEFAULT_WIDTH 512
#define DEFAULT_HEIGHT 512
#define WINDOW_NAME "Test"
#define FPS 165
#define MAPSIZE 32
#define TILESIZE 16
#define TILES 4

// ADTS
struct tile
{
    SDL_Rect wall;
    int type;
};
typedef struct tile Tile;

struct player
{
    int x, y;
    SDL_Rect rect;
};
typedef struct player Player;

#endif