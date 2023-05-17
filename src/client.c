#include "client.h"

void *networkUpdate(void *pAppIn)
{
    App *pApp = (App *)pAppIn;
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += 1;

    while (!pApp->exit)
    {
        sem_timedwait(&pApp->semaphore.updateNetwork, &timeout);
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 1;

    }

    printf("Exiting NetworkThread\n");
    
    pthread_exit(NULL);
    return NULL;
}