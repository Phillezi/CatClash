#include "app.h"

int main(int argc, char **args)
{
    App *pApp = createApplication();
    if (pApp)
        run(pApp);
    else
        printf("Failed To create application\n");
    destroyApplication(pApp);
    return 0;
}

App *createApplication()
{
    App *pNew_app = (App *)malloc(sizeof(App));
    if (!pNew_app)
    {
        printf("Failed to allocate memory for Application\n");
        return NULL;
    }
    pNew_app->pWindow = createWindow();
    if (!pNew_app->pWindow)
        return NULL;
    pNew_app->pNetwork = createNetwork();
    if (!pNew_app->pNetwork)
        return NULL;
    pNew_app->pGame = createGame(pNew_app);
    if (!pNew_app->pGame)
        return NULL;
    pNew_app->pMenu = createMenu(pNew_app);
    if (!pNew_app->pMenu)
        return NULL;
    pNew_app->exit = false;
    pNew_app->state = MENU;

    return pNew_app;
}

void destroyApplication(App *pApp)
{
    if (pApp->pMenu)
        destroyMenu(pApp->pMenu);
    if (pApp->pGame)
        destroyGame(pApp->pGame);
    if (pApp->pNetwork)
        destroyNetwork(pApp->pNetwork);
    if (pApp->pWindow)
        destroyWindow(pApp->pWindow);
    if (pApp)
        free(pApp);
}

void run(App *pApp)
{
    pthread_t renderThread;
    pthread_t inputThread;
    pthread_t networkThread;

    sem_init(&pApp->semaphore.updateWindow, 0, 1);
    sem_init(&pApp->semaphore.updateInput, 0, 1);
    sem_init(&pApp->semaphore.updateNetwork, 0, 1);

    pthread_create(&renderThread, NULL, renderUpdate, (void *)pApp);
    pthread_create(&inputThread, NULL, inputUpdate, (void *)pApp);
    pthread_create(&networkThread, NULL, networkUpdate, (void *)pApp);

    while (!pApp->exit)
    {

        SDL_Event event;
        if (SDL_WaitEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                sem_post(&pApp->semaphore.updateWindow);
                sem_post(&pApp->semaphore.updateInput);
                sem_post(&pApp->semaphore.updateNetwork);
                pApp->exit = true;
            }
            else
            {
                sem_post(&pApp->semaphore.updateInput);
            }
        }
    }

    pthread_join(renderThread, NULL);
    pthread_join(inputThread, NULL);
    pthread_join(networkThread, NULL);
}
