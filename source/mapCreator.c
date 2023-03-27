#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define MAP_FILEPATH "resources/map.txt"
#define SAVE_MAP_FILEPATH "resources/savedMap.txt"
#define DEFAULT_WIDTH 512
#define DEFAULT_HEIGHT 512
#define WINDOW_NAME "MapCreator v.2"
#define FPS 60
#define MAPSIZE 32
#define TILESIZE 16

struct tile
{
    SDL_Rect wall;
    int type;
};
typedef struct tile Tile;

void initMap(Tile map[]);
Tile createTile(int x, int y, int type);
void saveToFile(Tile map[]);

int main(int argv, char **args)
{
    int updateScreen = 1;
    int prevTime = 0;
    int windowWidth = DEFAULT_WIDTH, windowHeight = DEFAULT_HEIGHT;
    int red = 255, green = 255, blue = 255;
    Tile map[MAPSIZE * MAPSIZE];

    initMap(map);
    
    SDL_Window *pWindow = SDL_CreateWindow(WINDOW_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    if (!pWindow)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!pRenderer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *pSurface1 = IMG_Load("resources/Tile1.png");
    if (!pSurface1)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTextureTile1 = SDL_CreateTextureFromSurface(pRenderer, pSurface1);
    SDL_FreeSurface(pSurface1);
    if (!pTextureTile1)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Surface *pSurface2 = IMG_Load("resources/Tile2.png");
    if (!pSurface2)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTextureTile2 = SDL_CreateTextureFromSurface(pRenderer, pSurface2);
    SDL_FreeSurface(pSurface2);
    if (!pTextureTile2)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

SDL_Surface *pSurface3 = IMG_Load("resources/Tile3.png");
    if (!pSurface3)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTextureTile3 = SDL_CreateTextureFromSurface(pRenderer, pSurface3);
    SDL_FreeSurface(pSurface3);
    if (!pTextureTile3)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

SDL_Surface *pSurface4 = IMG_Load("resources/Tile4.png");
    if (!pSurface4)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTextureTile4 = SDL_CreateTextureFromSurface(pRenderer, pSurface4);
    SDL_FreeSurface(pSurface4);
    if (!pTextureTile4)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }
    int running = 1;

    while (running)
    {
        int deltaTime = SDL_GetTicks() - prevTime;
        if (deltaTime >= 1000 / FPS) // updates at a frequency of FPS
        {
            prevTime = SDL_GetTicks();
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    running = 0;
            }

            int mouseX, mouseY;
            int buttons = SDL_GetMouseState(&mouseX, &mouseY);

            const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
            if (currentKeyStates[SDL_SCANCODE_1])
            {
                map[((mouseY/TILESIZE * MAPSIZE) + (mouseX/TILESIZE))].type = 1;
            }
            else if (currentKeyStates[SDL_SCANCODE_2])
            {
                map[((mouseY/TILESIZE * MAPSIZE) + (mouseX/TILESIZE))].type = 2;
            }
            else if (currentKeyStates[SDL_SCANCODE_3])
            {
                map[((mouseY/TILESIZE * MAPSIZE) + (mouseX/TILESIZE))].type = 3;
            }
            else if (currentKeyStates[SDL_SCANCODE_4])
            {
                map[((mouseY/TILESIZE * MAPSIZE) + (mouseX/TILESIZE))].type = 4;
            }
            else if (currentKeyStates[SDL_SCANCODE_S] && currentKeyStates[SDL_SCANCODE_LCTRL])
            {
                saveToFile(map);
                printf("saved file");
            }
            else if (currentKeyStates[SDL_SCANCODE_DELETE])
            {
                for(int i = 0; i < MAPSIZE * MAPSIZE; i++){
                    map[i].type = 0;
                }
            }
            else if (currentKeyStates[SDL_SCANCODE_INSERT])
            {
                for(int i = 0; i < MAPSIZE * MAPSIZE; i++){
                    map[i].type = 1;
                }
            }

            if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                map[((mouseY/TILESIZE * MAPSIZE) + (mouseX/TILESIZE))].type = 1;
            }

            if (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))
            {
                map[((mouseY/TILESIZE * MAPSIZE) + (mouseX/TILESIZE))].type = 0;
            }
            

            SDL_SetRenderDrawColor(pRenderer, red, green, blue, 255);
            SDL_RenderClear(pRenderer);
            SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);

            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
            {
                switch (map[i].type)
                {
                case 0:
                    break;
                case 1:
                    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile1, NULL, &map[i].wall);//SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(pRenderer, 100, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile2, NULL, &map[i].wall);//SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(pRenderer, 0, 100, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile3, NULL, &map[i].wall);//SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 4:
                    SDL_SetRenderDrawColor(pRenderer, 0, 0, 100, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile4, NULL, &map[i].wall);//SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                default:
                    break;
                }
            }

            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            SDL_RenderPresent(pRenderer);
        }
    }
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
    return 0;
}

void initMap(Tile map[])
{
    int type = 0;
    // READ FROM map.txt
    FILE *fp;
    fp = fopen(MAP_FILEPATH, "r");
    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                type = 0;
                fscanf(fp, "%d", &type);
                map[row * MAPSIZE + col] = createTile(col * TILESIZE, row * TILESIZE, type);
            }
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR READING FILE");
    }
}
Tile createTile(int x, int y, int type)
{
    Tile i;
    i.wall.x = x;
    i.wall.y = y;
    i.wall.w = TILESIZE;
    i.wall.h = TILESIZE;
    i.type = type;
    return i;
}
void saveToFile(Tile map[]){
    FILE *fp;
    fp = fopen(SAVE_MAP_FILEPATH, "w+");
    if (fp != NULL)
    {
        for (int row = 0; row < MAPSIZE; row++)
        {
            for (int col = 0; col < MAPSIZE; col++)
            {
                fprintf(fp, "%d ", map[row * MAPSIZE + col].type);
            }
            fprintf(fp,"\n");
        }
        fclose(fp);
    }
    else
    {
        printf("ERROR opening FILE");
    }
}