#ifndef IOHANDLER_H
#define IOHANDLER_H

#include "definitions.h"
#include <dirent.h>

void checkFolder(char *folderPath);

char **checkFolderAndReturnList(char *folderPath, int *arrLen);

void freeTextList(char **strArr, int len);

#endif