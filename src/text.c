#include "text.h"


Text *createText(SDL_Renderer *pRenderer, int r, int g, int b, TTF_Font *pFont, char *pString, int x, int y)
{
    Text *pText = malloc(sizeof(Text));
    SDL_Color color = {r,g,b};
    SDL_Surface *pSurface = TTF_RenderText_Solid(pFont, pString, color);
    if(!pSurface){
        printf("Error: %s\n", SDL_GetError());
        return NULL;
    }
    pText->pTexture = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if(!pText->pTexture){
        printf("ERROR: %s\n", SDL_GetError());
        free(pText);
    }
    SDL_QueryTexture(pText->pTexture,NULL,NULL,&pText->rect.w, &pText->rect.h);
    pText->rect.x = x - pText->rect.w/2;
    pText->rect.y = y - pText->rect.h/2;

    return pText;
}

void drawText(Text *pText, SDL_Renderer *pRenderer){
    SDL_RenderCopy(pRenderer, pText->pTexture, NULL, &pText->rect);
}

void freeText(Text *pText){
    SDL_DestroyTexture(pText->pTexture);
    free(pText);
}
