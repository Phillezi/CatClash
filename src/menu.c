#include "menu.h"

Menu *createMenu(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    Menu *pNew_menu = (Menu *)malloc(sizeof(Menu));
    if(!pNew_menu)
    {
        printf("Failed to allocate memory for Menu\n");
        return NULL;
    }
    if(initalizeMenu(pNew_menu, pApp))
    {
        printf("Failed to initialize Menu\n");
        return NULL;
    }
    return pNew_menu;
}

void destroyMenu(Menu *pMenu)
{
    if(pMenu->pPlayText)
        freeText(pMenu->pPlayText);

    if(pMenu->pEditText)
        freeText(pMenu->pEditText);

    if(pMenu->pJoinText)
        freeText(pMenu->pJoinText);

    if(pMenu->pQuitText)
        freeText(pMenu->pQuitText);

    if(pMenu->pfont)
        TTF_CloseFont(pMenu->pfont);

    TTF_Quit();
    
    if(pMenu)
        free(pMenu);
}

int initalizeMenu(Menu *pMenu, void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    if(TTF_Init())
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    pMenu->selection = MENU_PLAY;

    pMenu->fontSize = pApp->pWindow->width / 16;

    pMenu->pfont = TTF_OpenFont(FONT_PATH, pMenu->fontSize);

    pMenu->pPlayText = createText(pApp->pWindow->pRenderer, 255, 0, 0, pMenu->pfont, "PLAY", pApp->pWindow->width/2, pMenu->fontSize);
    pMenu->pEditText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pMenu->pfont, "Edit", pApp->pWindow->width/2, 2*pMenu->fontSize);
    pMenu->pJoinText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pMenu->pfont, "Join", pApp->pWindow->width/2, 3*pMenu->fontSize);
    pMenu->pQuitText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pMenu->pfont, "Quit", pApp->pWindow->width/2, 4*pMenu->fontSize);

    return 0;
}