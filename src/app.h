#ifndef APP_H
#define APP_H

#include "adts.h"

#include "window.h"
#include "network.h"
#include "game.h"
#include "menu.h"
#include "render.h"
#include "input.h"
#include "client.h"


App *createApplication();

void destroyApplication (App *pApp);

void run (App *pApp);

#endif
