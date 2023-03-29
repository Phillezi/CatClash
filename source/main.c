#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define MAP_FILEPATH "resources/map.txt"
#define SAVE_MAP_FILEPATH "resources/savedMap.txt"
#define DEFAULT_WIDTH 512
#define DEFAULT_HEIGHT 512
#define WINDOW_NAME "Test"
#define FPS 165
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

int main(int argv, char **args)
{
    int collision = 0;
    int movementPrevTime = 0;
    int prevTime = 0;
    int movementAmount = 1;
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

    SDL_Surface *pSurface = IMG_Load("resources/cat3.png");
    if (!pSurface)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *pTexturePlayer = SDL_CreateTextureFromSurface(pRenderer, pSurface);
    SDL_FreeSurface(pSurface);
    if (!pTexturePlayer)
    {
        printf("Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(pRenderer);
        SDL_DestroyWindow(pWindow);
        SDL_Quit();
        return 1;
    }

    SDL_Rect playerRect;
    SDL_QueryTexture(pTexturePlayer, NULL, NULL, &playerRect.w, &playerRect.h);

    playerRect.x = windowWidth / 2;
    playerRect.y = windowHeight / 2;

    playerRect.w = TILESIZE;
    playerRect.h = TILESIZE;

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
            int movementDeltaTime = SDL_GetTicks() - movementPrevTime;
            if (movementDeltaTime >= 1000 / 60)
            {
                movementPrevTime = SDL_GetTicks();

                const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
                if (currentKeyStates[SDL_SCANCODE_R] || currentKeyStates[SDL_SCANCODE_DELETE]){
                        initMap(map);
                        playerRect.x = windowWidth / 2;
                        playerRect.y = windowHeight / 2;
                        printf("Reset\n");
                }
                if (currentKeyStates[SDL_SCANCODE_W] || currentKeyStates[SDL_SCANCODE_UP])
                {
                    collision = 0;
                    if(map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].type != 0){
                        map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].type = 2;
                        collision = 1;

                        printf("DEBUG: COLLISION W at player(%d,%d) and tile(%d,%d)\n",playerRect.x,playerRect.y, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.x, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.y);
                    }
                        

                    if (!collision)
                    {
                        if (1)//playerRect.y >= windowHeight / 4)
                        {
                            playerRect.y -= movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.y += movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT])
                {
                    collision = 0;
                    if(map[((playerRect.y/TILESIZE)*MAPSIZE)+((playerRect.x-movementAmount)/TILESIZE)].type != 0){
                        map[((playerRect.y/TILESIZE)*MAPSIZE)+((playerRect.x-movementAmount)/TILESIZE)].type = 2;
                        collision = 1;
                        printf("DEBUG: COLLISION A at player(%d,%d) and tile(%d,%d)\n",playerRect.x,playerRect.y, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.x, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.y);
                    }

                    if (!collision)
                    {
                        if (1)//playerRect.x >= windowWidth / 4)
                        {
                            playerRect.x -= movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.x += movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN])
                {
                    collision = 0;
                    if(map[(((playerRect.y+(TILESIZE-1)+movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].type != 0){
                        map[(((playerRect.y+(TILESIZE-1)+movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].type = 2;
                        collision = 1;
                        printf("DEBUG: COLLISION S at player(%d,%d) and tile(%d,%d)\n",playerRect.x,playerRect.y, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.x, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.y);
                    }

                    if (!collision)
                    {
                        if (1)//playerRect.y <= ((3 * windowHeight) / 4))
                        {
                            playerRect.y += movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.y -= movementAmount;
                            }
                        }
                    }
                }
                if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT])
                {
                    collision = 0;
                    if(map[((playerRect.y/TILESIZE)*MAPSIZE)+((playerRect.x+(TILESIZE-1)+movementAmount)/TILESIZE)].type != 0){
                        map[((playerRect.y/TILESIZE)*MAPSIZE)+((playerRect.x+(TILESIZE-1)+movementAmount)/TILESIZE)].type = 2;
                        collision = 1;
                        printf("DEBUG: COLLISION D at player(%d,%d) and tile(%d,%d)\n",playerRect.x,playerRect.y, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.x, map[(((playerRect.y-movementAmount)/TILESIZE)*MAPSIZE)+(playerRect.x/TILESIZE)].wall.y);
                    }

                    if (!collision)
                    {
                        if (1)//playerRect.x <= ((3 * windowWidth) / 4))
                        {
                            playerRect.x += movementAmount;
                        }
                        else
                        {
                            for (int i = 0; i < MAPSIZE * MAPSIZE; i++)
                            {
                                map[i].wall.x -= movementAmount;
                            }
                        }
                    }
                }
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
                    SDL_RenderCopy(pRenderer, pTextureTile1, NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 2:
                    SDL_SetRenderDrawColor(pRenderer, 100, 0, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile2, NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 3:
                    SDL_SetRenderDrawColor(pRenderer, 0, 100, 0, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile3, NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                case 4:
                    SDL_SetRenderDrawColor(pRenderer, 0, 0, 100, 255);
                    SDL_RenderCopy(pRenderer, pTextureTile4, NULL, &map[i].wall); // SDL_RenderFillRect(pRenderer, &map[i].wall);
                    break;
                default:
                    break;
                }
            }

            SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
            SDL_RenderDrawRect(pRenderer, &playerRect);
            SDL_RenderCopy(pRenderer, pTexturePlayer, NULL, &playerRect);
            SDL_RenderPresent(pRenderer);
        }
    }
    SDL_DestroyTexture(pTextureTile1);
    SDL_DestroyTexture(pTextureTile2);
    SDL_DestroyTexture(pTextureTile3);
    SDL_DestroyTexture(pTextureTile4);
    SDL_DestroyTexture(pTexturePlayer);
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