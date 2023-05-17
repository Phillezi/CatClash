#include "input.h"

void *inputUpdate(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;

    while (!pApp->exit)
    {
        sem_timedwait(&pApp->semaphore.updateInput, &timeout);
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 1;

        handleInput((void *)pApp);

        sem_post(&pApp->semaphore.updateWindow);
    }

    printf("Exiting InputThread\n");

    pthread_exit(NULL);
    return NULL;
}

void handleInput(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    switch (pApp->state)
    {
    case MENU:
        handleMenuInput(pAppIn);
        break;
    case PLAY:
        handleGameInput(pAppIn);
        break;
    default:
        handleGameInput(pAppIn);
        break;
    }
}

void handleMenuInput(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    int mouseX, mouseY;
    Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

    int previousMenuSelection = pApp->pMenu->selection;

    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
        if (pApp->pMenu->selection > MENU_PLAY)
            pApp->pMenu->selection--;
        else
            pApp->pMenu->selection = MENU_QUIT;
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
        if (pApp->pMenu->selection < MENU_QUIT)
            pApp->pMenu->selection++;
        else
            pApp->pMenu->selection = MENU_PLAY;
    }
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    {
    }
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    {
    }
    if (keys[SDL_SCANCODE_ESCAPE])
    {
        pApp->pMenu->selection = MENU_QUIT;
    }
    if (keys[SDL_SCANCODE_RETURN2] || keys[SDL_SCANCODE_RETURN])
    {
        pApp->state = pApp->pMenu->selection;
    }
    if (pApp->pMenu->selection != previousMenuSelection)
    {
        switch (previousMenuSelection)
        {
        case MENU_PLAY:
            freeText(pApp->pMenu->pPlayText);
            pApp->pMenu->pPlayText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pApp->pMenu->pfont, "Play", pApp->pWindow->width / 2, pApp->pMenu->fontSize);
            break;
        case MENU_EDIT:
            freeText(pApp->pMenu->pEditText);
            pApp->pMenu->pEditText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pApp->pMenu->pfont, "Edit", pApp->pWindow->width / 2, 2 * pApp->pMenu->fontSize);
            break;
        case MENU_JOIN:
            freeText(pApp->pMenu->pJoinText);
            pApp->pMenu->pJoinText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pApp->pMenu->pfont, "Join", pApp->pWindow->width / 2, 3 * pApp->pMenu->fontSize);
            break;
        case MENU_QUIT:
            freeText(pApp->pMenu->pQuitText);
            pApp->pMenu->pQuitText = createText(pApp->pWindow->pRenderer, 0, 0, 0, pApp->pMenu->pfont, "Quit", pApp->pWindow->width / 2, 4 * pApp->pMenu->fontSize);
            break;
        }

        switch (pApp->pMenu->selection)
        {
        case MENU_PLAY:
            freeText(pApp->pMenu->pPlayText);
            pApp->pMenu->pPlayText = createText(pApp->pWindow->pRenderer, 255, 0, 0, pApp->pMenu->pfont, "PLAY", pApp->pWindow->width / 2, pApp->pMenu->fontSize);
            break;
        case MENU_EDIT:
            freeText(pApp->pMenu->pEditText);
            pApp->pMenu->pEditText = createText(pApp->pWindow->pRenderer, 255, 0, 0, pApp->pMenu->pfont, "EDIT", pApp->pWindow->width / 2, 2 * pApp->pMenu->fontSize);
            break;
        case MENU_JOIN:
            freeText(pApp->pMenu->pJoinText);
            pApp->pMenu->pJoinText = createText(pApp->pWindow->pRenderer, 255, 0, 0, pApp->pMenu->pfont, "JOIN", pApp->pWindow->width / 2, 3 * pApp->pMenu->fontSize);
            break;
        case MENU_QUIT:
            freeText(pApp->pMenu->pQuitText);
            pApp->pMenu->pQuitText = createText(pApp->pWindow->pRenderer, 255, 0, 0, pApp->pMenu->pfont, "QUIT", pApp->pWindow->width / 2, 4 * pApp->pMenu->fontSize);
            break;
        }
    }
}

void handleGameInput(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
    }
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
    {
    }
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
    {
    }
    if (keys[SDL_SCANCODE_ESCAPE])
    {
        pApp->state = MENU;
    }
    if (keys[SDL_SCANCODE_RETURN2] || keys[SDL_SCANCODE_RETURN])
    {
    }
}
