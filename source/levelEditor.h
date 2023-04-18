#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "definitions.h"

int levelEditor(Game *pGame);
void lvlhandleZoom(Game *pGame, int mouseWheelY);
void lvlhandleInput(Game *pGame, int *pMouseX, int *pMouseY);
void lvlupdateScreen(Game *pGame, int mouseX, int mouseY);
void saveToFile(Tile map[], char fileName[]);

#endif