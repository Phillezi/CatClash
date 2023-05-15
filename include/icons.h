#include "definitions.h"
#include "text.h"

typedef struct icon{
    SDL_Rect rect;
    SDL_Color color;
    Text *pText;
    bool textPresent;
} Icon;

Icon *createIcons(Icon *pIcon, Uint32 size, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a, char *pString, TTF_Font *pFont, SDL_Renderer *pRenderer);

void removeIcon(Icon icon);

void destroyIcons(Icon *pIcon, Uint32 size);

void drawIcons(Icon *pIcons, Uint32 size, SDL_Renderer *pRenderer);