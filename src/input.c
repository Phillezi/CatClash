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

    }

    printf("Exiting InputThread\n");
    
    pthread_exit(NULL);
    return NULL;
}