#include "render.h"

void *renderUpdate(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;
    pApp->pWindow->frameCounter = 0;

    while (!pApp->exit)
    {
        if (sem_timedwait(&pApp->semaphore.updateWindow, &timeout))
        {
            clock_gettime(CLOCK_REALTIME, &timeout);
            timeout.tv_sec += 1;
            pApp->pWindow->fps = pApp->pWindow->frameCounter;
            pApp->pWindow->frameCounter = 0;
            char buffer[31];
            sprintf(buffer, "FPS: %d\n", pApp->pWindow->fps);
            SDL_SetWindowTitle(pApp->pWindow->pWindow, buffer);
        }

        render((void *)pApp);
        pApp->pWindow->frameCounter++;
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

    switch (pApp->state)
    {
    case MENU:
        renderMenu(pAppIn);
        break;
    case PLAY:
        renderGame(pAppIn);
        break;
    case EDIT:
        renderGame(pAppIn);
        break;
    case JOIN:
        renderGame(pAppIn);
        break;
    case QUIT:
        break;
    }

    SDL_RenderPresent(pRenderer);
}

void renderMenu(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    SDL_Renderer *pRenderer = pApp->pWindow->pRenderer;

    SDL_Rect rect = {100, 100, 100, 100};

    drawText(pApp->pMenu->pPlayText, pRenderer);
    drawText(pApp->pMenu->pEditText, pRenderer);
    drawText(pApp->pMenu->pJoinText, pRenderer);
    drawText(pApp->pMenu->pQuitText, pRenderer);

    SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(pRenderer, &rect);
}

void renderGame(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    SDL_Renderer *pRenderer = pApp->pWindow->pRenderer;
    Game *pGame = pApp->pGame;

    SDL_Rect temp = {0, 0, pApp->pWindow->width, pApp->pWindow->height};
    SDL_SetRenderDrawColor(pRenderer, 0, 255, 0, 255);
    SDL_RenderFillRect(pRenderer, &temp);
}