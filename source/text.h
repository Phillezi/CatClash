#include "definitions.h"
#ifndef text_h
#define text_h


Text *createText(SDL_Renderer *pRenderer, int r, int g, int b, TTF_Font *pFont, char *pString, int x, int y);
void drawText(Text *pText);
void freeText(Text *pText);

#endif