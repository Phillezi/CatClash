#include "render.h"

void *renderUpdate(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;

    while (!pApp->exit)
    {
        sem_timedwait(&pApp->semaphore.updateWindow, &timeout);
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 1;

        render(pApp);
    }

    printf("Exiting RenderThread\n");

    pthread_exit(NULL);
    return NULL;
}

void render(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    SDL_Renderer *pRenderer = pApp->pWindow->pRenderer;

    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    SDL_RenderClear(pRenderer);
    
    switch(pApp->state)
    {
        case MENU: renderMenu(pApp); break;
        case PLAY: renderGame(pApp); break;
    }

    SDL_RenderPresent(pRenderer);
}

void renderMenu(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    SDL_Renderer *pRenderer = pApp->pWindow->pRenderer;

    SDL_Rect rect = {100, 100, 100, 100};

    SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(pRenderer, &rect);

}

void renderGame(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    SDL_Renderer *pRenderer = pApp->pWindow->pRenderer;

    SDL_Rect rect = {100, 100, 100, 100};

    SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
    SDL_RenderFillRect(pRenderer, &rect);

}