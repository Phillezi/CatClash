#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define OS_RECOGNIZED 1
#define COMMAND(x) "start " x
#else
#ifdef __linux__
#define OS_RECOGNIZED 1
#define COMMAND(x) "./" x
#else
#ifdef __APPLE__
#define OS_RECOGNIZED 1
#define COMMAND(x) "./" x
#else
#define OS_RECOGNIZED 0
#endif
#endif
#endif

/*
    I have not tested this on linux or macos
*/

int main() {
    if (OS_RECOGNIZED)
    {
        for(int i = 0; i < 4; i++)
        system(COMMAND("main.exe"));
    system(COMMAND("betterServer.exe"));
    }
    else
    {
        printf("OS is not recognized, check gameLauncher.c to edit precompiler definition checks\n");
    }
    
    return 0;
}
