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

        SDL_SetRenderDrawColor(pApp->pWindow->pRenderer, 0, 0, 0, 255);
        SDL_RenderClear(pApp->pWindow->pRenderer);
        SDL_SetRenderDrawColor(pApp->pWindow->pRenderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(pApp->pWindow->pRenderer, 0, 0, pApp->pWindow->width, pApp->pWindow->height);

        SDL_RenderPresent(pApp->pWindow->pRenderer);
    }

    printf("Exiting RenderThread\n");
    
    pthread_exit(NULL);
    return NULL;
}