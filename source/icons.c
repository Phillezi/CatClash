#include "../include/icons.h"

Icon *createIcons(Icon *pIcon, Uint32 size, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a, char *pString, TTF_Font *pFont, SDL_Renderer *pRenderer)
{

    Icon *pNewIcon = NULL;
    if (!pIcon)
    {
        pNewIcon = (Icon *)malloc(sizeof(Icon));

        if (!pNewIcon)
        {
            printf("ERROR, could not allocate memory for icon\n");
            return pIcon;
        }

        pNewIcon->rect.x = x;
        pNewIcon->rect.y = y;
        pNewIcon->rect.w = w;
        pNewIcon->rect.h = h;

        pNewIcon->color.r = r;
        pNewIcon->color.g = g;
        pNewIcon->color.b = b;
        pNewIcon->color.a = a;

        if (pString[0])
        {
            pNewIcon->pText = createText(pRenderer, 255 - r, 255 - g, 255 - b, pFont, pString, x + (w / 2), y + (h / 2));
            pNewIcon->textPresent = true;
        }
        else
        {
            pNewIcon->textPresent = false;
        }

    }
    else
    {
        pNewIcon = (Icon *)realloc(pIcon, size + 1 * sizeof(Icon));

        if (!pNewIcon)
        {
            printf("ERROR, could not expand memory for icon\n");
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

        if (pString[0])
        {
            pNewIcon[size].pText = createText(pRenderer, 255 - r, 255 - g, 255 - b, pFont, pString, x + (w / 2), y + (h / 2));
            pNewIcon[size].textPresent = true;
        }
        else
        {
            pNewIcon[size].textPresent = false;
        }

    }

    return pNewIcon;
}

void removeIcon(Icon icon)
{
    if (icon.textPresent)
        freeText(icon.pText);
}

void destroyIcons(Icon *pIcon, Uint32 size)
{
    for (int i = 0; i < size; i++)
    {
        removeIcon(pIcon[i]);
    }
    free(pIcon);
}

void drawIcons(Icon *pIcons, Uint32 size, SDL_Renderer *pRenderer)
{
    for (int i = 0; i < size; i++)
    {
        if (pIcons[i].textPresent)
            drawText(pIcons[i].pText, pRenderer);
        SDL_RenderFillRect(pRenderer, &pIcons[i].rect);
    }
}