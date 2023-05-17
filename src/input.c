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

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        if(keys[SDL_SCANCODE_UP])
        {
            if(pApp->state < PLAY)
                pApp->state++;
            else
                pApp->state = MENU;
        }
        
        sem_post(&pApp->semaphore.updateWindow);

    }

    printf("Exiting InputThread\n");
    
    pthread_exit(NULL);
    return NULL;
}