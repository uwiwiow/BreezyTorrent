#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <stdlib.h>
#include <time.h>

// Define screen dimensions
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   320
#define AVANZE          4

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
    int max_x = SCREEN_WIDTH / 20;

    // Generar un número aleatorio dentro del rango [0, max] y multiplicarlo por 20
    int ran_x = (rand() % (max_x)) * 20;

    return ran_x;
}


int ran_y()
{
    int max_y = SCREEN_HEIGHT / 20;
    int ran_y = (rand() % (max_y)) * 20;
    return ran_y;
}


void stabb(SDL_Rect *squareRect, bool x)
{
    if (x)
    {
        int num = squareRect->x;
        num = num % 100; // obtener ultimas dos cifras
        int pc = num / 10;
        int sc = num % 10;
        if (pc % 2 == 0)
        {
            squareRect->x = squareRect->x -sc;
        } else
        {
            squareRect->x = squareRect->x + (10-sc);
        }
    } else
    {
        int num = squareRect->y;
        num = num % 100;
        int pc = num / 10;
        int sc = num % 10;
        if (pc % 2 == 0)
        {
            squareRect->y = squareRect->y -sc;
        } else
        {
            squareRect->y = squareRect->y + (10-sc);
        }
    }
}


SDL_Rect save_pos(SDL_Rect rect)
{
    return rect;
}

int dist_xaxis(SDL_Rect bodyRect, SDL_Rect foodRect)
{
    int avanceBloq = (foodRect.x / 20) - (bodyRect.x / 20);

    return avanceBloq;
}

bool for_xaxis(SDL_Rect bodyRect, SDL_Rect foodRect)
{
    int avanceBloq = (foodRect.x / 20) - ((bodyRect.x / 20) + 20);
    bool ret;

    //si es positivo && si es mayor que 8
    if ((avanceBloq > 8) || (avanceBloq < 0 && avanceBloq > -8))
    {
        ret = true;
    }
    if ((avanceBloq < 8 && avanceBloq > 0) || avanceBloq < -8)
    {
        ret = false;
    }

    // true es abajo
    return ret;
}

bool for_yaxis(SDL_Rect bodyRect, SDL_Rect foodRect)
{
    int avanceBloq = (foodRect.y / 20) - (bodyRect.y / 20);
    bool ret;

    //si es positivo && si es mayor que 8
    if ((avanceBloq > 8) || (avanceBloq < 0 && avanceBloq > -8))
    {
        ret = true;
    }
    if ((avanceBloq < 8 && avanceBloq > 0) || avanceBloq < -8)
    {
        ret = false;
    }

    // true es abajo
    return ret;
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
    const bool walldeath = false;

    //bonus inicial
    int bonus = 2;

    // bot mode
    bool bot_mode = true;

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

            // Square dimensions
            squareRect.w = 20;
            squareRect.h = 20;

            // Square position
            squareRect.x = 0;
            squareRect.y = 0;

            bool up = false;
            bool down = false;
            bool left = false;
            bool right = true;

            SDL_Rect foodRect;
            foodRect.w = 20;
            foodRect.h = 20;
            foodRect.x = ran_x();
            foodRect.y = ran_y();

            //      Cuerpo

            // Vector para almacenar las posiciones antiguas
            const int MAX_POSITIONS = (255 * (20/AVANZE));
            Position positions[MAX_POSITIONS];
            int positionCount = 0;

            // Lista de rects
            SDL_Rect rectList[MAX_POSITIONS];
            int rectCount = 0;

            // checar si quiere sobreponerse despues de un giro
            SDL_Rect saved_rect;

            // guardar movimiento
            int letter;
            int act_letter;

            // Event loop exit flag
            bool quit = false;

            SDL_bool a_pressed = SDL_FALSE;
            SDL_bool s_pressed = SDL_FALSE;
            SDL_bool d_pressed = SDL_FALSE;
            SDL_bool w_pressed = SDL_FALSE;

            SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
            SDL_EventState(SDL_MOUSEBUTTONUP, SDL_IGNORE);
            SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
            SDL_EventState(SDL_MOUSEWHEEL, SDL_IGNORE);

            int iter = 0;
            int disaxis;

            SDL_Rect savedfoodRect = foodRect;


            // Event loop
            while(!quit)
            {
                frameStart = SDL_GetTicks();
                SDL_Event e;

                if (bot_mode)
                {
                    disaxis = dist_xaxis(squareRect, foodRect);
                    printf("dix     %d ----------\n",disaxis);
                    if (disaxis != 0)
                    {
                        e.type = SDL_KEYDOWN;
                        if (for_xaxis(squareRect, foodRect))
                        {
                            e.key.keysym.sym = SDLK_a;
                        } else
                        {
                            e.key.keysym.sym = SDLK_d;
                        }
                        SDL_PushEvent(&e);
                    }
                    if (disaxis == 0){
                        e.type = SDL_KEYDOWN;
                        if (for_yaxis(squareRect, foodRect))
                        {
                            e.key.keysym.sym = SDLK_w;
                        } else
                        {
                            e.key.keysym.sym = SDLK_s;
                        }
                        SDL_PushEvent(&e);
                    }
                }

                printf("%d,%d\n",squareRect.x/20,squareRect.y/20);
                printf("%d,%d\n",foodRect.x/20,foodRect.y/20);
                printf("%d,%d       %d\n\n",squareRect.x,squareRect.y, rectCount/5);

                while (SDL_PollEvent(&e)) {

                    // User requests quit
                    if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
                    {
                        quit = true;
                    }
                    else if (e.type == SDL_KEYDOWN) {
                        if(!((saved_rect.x/20) == (squareRect.x/20) && (saved_rect.y/20) == (squareRect.y/20))) {
                            if (bot_mode)
                            {
                                a_pressed = SDL_FALSE;
                                s_pressed = SDL_FALSE;
                                d_pressed = SDL_FALSE;
                                w_pressed = SDL_FALSE;
                            }
                            if (e.key.keysym.sym == SDLK_a && !a_pressed && left == false && right == false) {
                                // 97
                                set_true(&left, &up, &down, &right, &left);
                                stabb(&squareRect, false);
                                saved_rect = save_pos(squareRect);
                                saved_rect.x -= 1;
                                act_letter = 97;
                                a_pressed = SDL_TRUE;
                                printf("left\n");
                                break;
                            } else if (e.key.keysym.sym == SDLK_s && !s_pressed && down == false && up == false) {
                                //115
                                set_true(&down, &up, &down, &right, &left);
                                stabb(&squareRect, true);
                                saved_rect = save_pos(squareRect);
                                act_letter = 115;
                                s_pressed = SDL_TRUE;
                                printf("down\n");
                                break;
                            } else if (e.key.keysym.sym == SDLK_d && !d_pressed && right == false && left == false) {
                                // 100
                                set_true(&right, &up, &down, &right, &left);
                                stabb(&squareRect, false);
                                saved_rect = save_pos(squareRect);
                                act_letter = 100;
                                d_pressed = SDL_TRUE;
                                printf("right\n");
                                break;
                            } else if (e.key.keysym.sym == SDLK_w && !w_pressed && up == false && down == false) {
                                // 119
                                set_true(&up, &up, &down, &right, &left);
                                stabb(&squareRect, true);
                                saved_rect = save_pos(squareRect);
                                saved_rect.y -= 1;
                                act_letter = 119;
                                w_pressed = SDL_TRUE;
                                printf("up\n");
                                break;
                            }
                        } else
                        {
                            if (act_letter != e.key.keysym.sym)
                            {
                                letter = e.key.keysym.sym;
                                SDL_Event event;
                                event.type = SDL_KEYDOWN;
                                event.key.keysym.sym = letter;
                                SDL_PushEvent(&event);
                            }

                        }
                    }
                    else if (e.type == SDL_KEYUP) {
                        if (e.key.keysym.sym == SDLK_a) {
                            a_pressed = SDL_FALSE;
                        }
                        else if (e.key.keysym.sym == SDLK_s) {
                            s_pressed = SDL_FALSE;
                        }
                        else if (e.key.keysym.sym == SDLK_d) {
                            d_pressed = SDL_FALSE;
                        }
                        else if (e.key.keysym.sym == SDLK_w) {
                            w_pressed = SDL_FALSE;
                        }
                    }

                }

                if (squareRect.x == SCREEN_WIDTH)
                {
                    if (walldeath)
                    {
                        quit = true;
                    } else
                    {
                        squareRect.x = 0;
                    }
                }
                if (squareRect.x == -20)
                {
                    if (walldeath)
                    {
                        quit = true;
                    } else
                    {
                        squareRect.x = SCREEN_WIDTH - 20;
                    }
                }
                if (squareRect.y == SCREEN_HEIGHT)
                {
                    if (walldeath)
                    {
                        quit = true;
                    } else
                    {
                        squareRect.y = 0;
                    }
                }
                if (squareRect.y == -20)
                {
                    if (walldeath)
                    {
                        quit = true;
                    } else
                    {
                        squareRect.y = SCREEN_HEIGHT - 20;
                    }
                }

                // Guardar la posición actual en el vector
                Position newPosition;
                newPosition.x = squareRect.x;
                newPosition.y = squareRect.y;

                if (up)
                {
                    squareRect.y -= AVANZE;
                }
                if (down)
                {
                    squareRect.y += AVANZE;
                }
                if (left)
                {
                    squareRect.x -= AVANZE;
                }
                if (right)
                {
                    squareRect.x += AVANZE;
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


                if (bonus > 0)
                {
                    for (int i = 0; i < (20 / AVANZE); ++i) {
                        SDL_Rect newRect;

                        // Agregar el nuevo rect a la lista
                        rectList[rectCount] = newRect;
                        rectCount++;
                    }
                }
                bonus--;


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
                        for (int i = 0; i < (20 / AVANZE); ++i) {
                            SDL_Rect newRect;

                            // Agregar el nuevo rect a la lista
                            rectList[rectCount] = newRect;
                            rectCount++;
                        }
                    }

                }

                // Renderizar todos los rects del cuerpo
                SDL_SetRenderDrawColor(renderer, 0x00, 0xAA, 0x00, 0xFF);
                for (int i = 0; i < rectCount; i++) {
                    rectList[i].x = positions[i].x;
                    rectList[i].y = positions[i].y;
                    rectList[i].w = 20;
                    rectList[i].h = 20;
                    SDL_RenderFillRect(renderer, &rectList[i]);

                    // revisar si se transpone para terminar el juego
                    if (squareRect.x == rectList[i].x && squareRect.y == rectList[i].y)
                    {
                        quit = true;
                    }

                    // revisar si alguna parte del cuerpo se transpone con la comida
                    if (rectList[i].x == foodRect.x && rectList[i].y == foodRect.y)
                    {
                        foodRect.x = ran_x();
                        foodRect.y = ran_y();

                        if (rectCount < sizeof(rectList)) {
                            // Crear un nuevo rect
                            for (int j = 0; j < (20 / AVANZE); ++j) {
                                SDL_Rect newRect;

                                // Agregar el nuevo rect a la lista
                                rectList[rectCount] = newRect;
                                rectCount++;
                            }
                        }

                    }

                }

                // Update screen
                SDL_RenderPresent(renderer);

                frameTime = SDL_GetTicks64() - frameStart;

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
