#include <stdlib.h>
#include <string.h>
#include "../include/ioHandler.h"

void checkFolder(char *folderPath)
{
    DIR *directory;
    struct dirent *dir;
    directory = opendir(folderPath);
    if (directory)
    {
        int i = 0;
        while ((dir = readdir(directory)) != NULL)
        {

            printf("%s\n", dir->d_name);
            i++;
        }
        closedir(directory);
    }
}

char **checkFolderAndReturnList(char *folderPath , int *arrLen)
{
    DIR *directory;
    struct dirent *dir;
    directory = opendir(folderPath);
    int i = 0;
    char **fileNames = malloc(100 * sizeof(char *));
    if (directory)
    {

        while ((dir = readdir(directory)) != NULL)
        {
            if (strstr(dir->d_name, ".txt") && i < 100)
            {
                fileNames[i] = (char *)malloc((strlen(dir->d_name)-strlen(".txt") + 1) * sizeof(char));
                if(!fileNames[i]){
                    printf("Error: Could not allocate memory\n");
                    break;
                }
                dir->d_name[(strlen(dir->d_name)-strlen(".txt"))] = 0; // remove ".txt"
                strcpy(fileNames[i], dir->d_name);
                i++;
            }
        }
        closedir(directory);
    }
    if (i)
    {
        *arrLen = i;
        return fileNames;
    }
    else
    {
        *arrLen = i;
        return NULL;
    }
}

void freeTextList(char **strArr, int len){
    for(int i = 0; i < len; i++){
        if(strArr[i]){
            //printf("Trying to free memory at: %p\n", strArr[i]);
            free(strArr[i]);
            //printf("Success!\n");
        }
        else{
            printf("No adress at location index %d\n", i);
        }
            
    }
}