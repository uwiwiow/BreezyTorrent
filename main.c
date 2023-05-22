#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <stdlib.h>
#include <time.h>

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y)) el operador ? devuele el primer valor si es verdadero
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Define screen dimensions
#define SCREEN_WIDTH    1600
#define SCREEN_HEIGHT   900

typedef struct {
    int x;
    int y;
} Position;

void set_true(bool* direction, bool* up, bool* down, bool* right, bool* left)
{
    *up = false;
    *down = false;
    *right = false;
    *left = false;

    *direction = true;
}


int ran_x()
{
    // Definir el máximo del rango deseado
    int max_x = 80;

    // Generar un número aleatorio dentro del rango [0, max] y multiplicarlo por 20
    int ran_x = (rand() % (max_x + 1)) * 20;

    return ran_x;
}


int ran_y()
{
    int max_y = 45;
    int ran_y = (rand() % (max_y + 1)) * 20;
    return ran_y;
}


int main(int argc, char* argv[])
{
    // Unused argc, argv
    (void) argc;
    (void) argv;

    // iniciar semilla de numeros pseudoaleatorios
    srand(time(NULL));

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        return 0;
    }
#endif

    int FPS = 20;

    const int DELAY_TIME = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    // Create window
    SDL_Window *window = SDL_CreateWindow("Snake",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!window)
    {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            printf("Renderer could not be created!\n"
                   "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            // Declare rect of square
            SDL_Rect squareRect;

            // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
            squareRect.w = 20;
            squareRect.h = 20;

            // Square position: In the middle of the screen
            squareRect.x = (SCREEN_WIDTH / 2 - squareRect.w / 2) + 10;
            squareRect.y = SCREEN_HEIGHT / 2 - squareRect.h / 2;

            bool up = false;
            bool down = false;
            bool left = false;
            bool right = true;

            // Event loop exit flag
            bool quit = false;

            SDL_Rect foodRect;
            foodRect.w = 20;
            foodRect.h = 20;
            foodRect.x = ran_x();
            foodRect.y = ran_y();

            //      Cuerpo

            // Lista de rects
            SDL_Rect rectList[255];
            int rectCount = 0;

            // Vector para almacenar las posiciones antiguas
            const int MAX_POSITIONS = 255;
            Position positions[MAX_POSITIONS];
            int positionCount = 0;

            // Event loop
            while(!quit)
            {
                frameStart = SDL_GetTicks();
                SDL_Event e;

                while (SDL_PollEvent(&e)) {

                    // User requests quit
                    if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                    {
                        quit = true;
                    }
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_a && right == false)
                    {
                        set_true(&left, &up, &down, &right, &left);
                        break;
                    }
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_d && left == false)
                    {
                        set_true(&right, &up, &down, &right, &left);
                        break;
                    }
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_w && down == false)
                    {
                        set_true(&up, &up, &down, &right, &left);
                        break;
                    }
                    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_s && up == false)
                    {
                        set_true(&down, &up, &down, &right, &left);
                        break;
                    }

                }


                if (squareRect.x == 1600)
                {
                    squareRect.x = 0;
                }
                if (squareRect.x == -20)
                {
                    squareRect.x = 1580;
                }
                if (squareRect.y == 900)
                {
                    squareRect.y = 0;
                }
                if (squareRect.y == -20)
                {
                    squareRect.y = 880;
                }

                // Guardar la posición actual en el vector
                Position newPosition;
                newPosition.x = squareRect.x;
                newPosition.y = squareRect.y;

                if (up)
                {
                    squareRect.y -= 20;
                }
                if (down)
                {
                    squareRect.y += 20;
                }
                if (left)
                {
                    squareRect.x -= 20;
                }
                if (right)
                {
                    squareRect.x += 20;
                }

                // Desplazar los elementos existentes hacia la derecha
                for (int i = positionCount - 1; i >= 0; i--) {
                    positions[i + 1] = positions[i];
                }

                // Agregar la nueva posición al inicio del vector
                positions[0] = newPosition;

                // Incrementar el contador de posiciones si no se ha alcanzado el máximo
                if (positionCount < MAX_POSITIONS) {
                    positionCount++;
                }


                // 0x declares hex number   FF= 255
                // Initialize renderer color white for the background
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                // Clear screen
                SDL_RenderClear(renderer);

                // Set renderer color red to draw the square
                SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0x00, 0xFF);
                // Draw filled square
                SDL_RenderFillRect(renderer, &squareRect);

                // FOOD
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 0xFF);

                if (squareRect.x != foodRect.x || squareRect.y != foodRect.y)
                {
                    SDL_RenderFillRect(renderer, &foodRect);
                }
                if (squareRect.x == foodRect.x && squareRect.y == foodRect.y)
                {
                    foodRect.x = ran_x();
                    foodRect.y = ran_y();

                    if (rectCount < sizeof(rectList)) {
                        // Crear un nuevo rect
                        SDL_Rect newRect;

                        // Agregar el nuevo rect a la lista
                        rectList[rectCount] = newRect;
                        rectCount++;
                    }

                }
                //aqui quito cosas xd

                // Renderizar todos los rects del cuerpo
                SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0x00, 0xFF);
                for (int i = 0; i < rectCount; i++) {
                    rectList[i].x = positions[i].x;
                    rectList[i].y = positions[i].y;
                    rectList[i].w = 20;
                    rectList[i].h = 20;
                    if (squareRect.x == rectList[i].x && squareRect.y == rectList[i].y)
                    {
                        quit = true;
                    }
                    SDL_RenderFillRect(renderer, &rectList[i]);
                }

                // Update screen
                SDL_RenderPresent(renderer);

                frameTime = SDL_GetTicks() - frameStart;

                // Esperar el tiempo restante hasta alcanzar el frame rate deseado
                if (frameTime < DELAY_TIME) {
                    SDL_Delay(DELAY_TIME - frameTime);
                }
            }

            // Destroy renderer
            SDL_DestroyRenderer(renderer);
        }

        // Destroy window
        SDL_DestroyWindow(window);
    }

    // Quit SDL
    SDL_Quit();


    return 0;
}
