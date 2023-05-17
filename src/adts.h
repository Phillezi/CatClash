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


typedef struct {
    SDL_Rect rect;
    SDL_Texture *pTexture;
    SDL_Renderer *pRenderer;
} Text;


typedef struct {
    Uint32 width, height;
    SDL_Window *pWindow;
    SDL_Renderer *pRenderer;
    
} Window;

typedef enum {
    MENU_PLAY,
    MENU_EDIT,
    MENU_JOIN,
    MENU_QUIT
} Selection;

typedef struct {
    Selection selection;
    Uint32 fontSize;
    TTF_Font *pfont;
    Text *pPlayText;
    Text *pEditText;
    Text *pJoinText;
    Text *pQuitText;
} Menu;

typedef struct {
    
} Game;

typedef struct {
    
} Network;

typedef enum {
    PLAY,
    EDIT,
    JOIN,
    QUIT,
    MENU
} State;

typedef struct {
    sem_t updateWindow;
    sem_t updateInput;
    sem_t updateNetwork;
} Semaphores;

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