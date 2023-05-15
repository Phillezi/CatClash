#include "../include/icons.h"

Icon *createIcons(Icon *pIcon, Uint32 size, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a, char *pString, TTF_Font *pFont, SDL_Renderer *pRenderer)
{

    Icon *pNewIcon;
    if (!pIcon)
        pNewIcon = (Icon *)malloc(sizeof(Icon));
    else
        pNewIcon = (Icon *)realloc(pIcon, size + 1 * sizeof(Icon));

    if(!pNewIcon)
    {
        printf("ERROR, could not allocate memory for icon\n");
        return pIcon;
    }
        

    pNewIcon[size].rect.x = x;
    pNewIcon[size].rect.y = y;
    pNewIcon[size].rect.w = w;
    pNewIcon[size].rect.h = h;

    pNewIcon[size].color.r = r;
    pNewIcon[size].color.g = g;
    pNewIcon[size].color.b = b;
    pNewIcon[size].color.a = a;

    if(pString[0])
    {
        pNewIcon[size].pText = createText(pRenderer, 255 - r, 255 - g, 255 - b, pFont, pString, x + (w / 2), y + (h / 2));
        pNewIcon[size].textPresent = true;
    }
    else
    {
        pNewIcon[size].textPresent = false;
    }
        

    return pNewIcon;
}

void removeIcon(Icon icon)
{
    if(icon.textPresent)
        freeText(icon.pText);
}

void destroyIcons(Icon *pIcon, Uint32 size)
{
    for(int i = 0; i < size; i++)
    {
        removeIcon(pIcon[i]);
    }
    free(pIcon);
}