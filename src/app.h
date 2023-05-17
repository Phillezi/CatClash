#ifndef APP_H
#define APP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "window.h"
#include "network.h"
#include "game.h"
#include "render.h"

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
    Semaphores semaphore;
} App;


App *createApplication();

void destroyApplication (App *pApp);

void run (App *pApp);

#endif
