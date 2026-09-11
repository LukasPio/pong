#include <stdio.h>
#include <inttypes.h>
#include <SDL2/SDL.h>

#define SDL_ERROR 1

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_HORIZONTAL_GAP 20
#define SCREEN_VERTICAL_GAP 10

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 15

#define BACKGROUND_COLOR 0, 0, 0, SDL_ALPHA_OPAQUE
#define SPRITE_COLOR 255, 165, 0, SDL_ALPHA_OPAQUE

void on_sdl_error(void);
void setup_graphics(void);
void setup_paddles(void);
void render(void);
void handle_input(void);

SDL_Window *w = NULL;
SDL_Renderer *r = NULL;

uint8_t running = 1;

SDL_Rect player = {0};
SDL_Rect computer = {0};

int main()
{
    setup_graphics();
    setup_paddles();

    while (running)
    {
        render();
        handle_input();
    }

    SDL_Quit();
}

void on_sdl_error(void)
{
    printf("Ocurred an error: %s", SDL_GetError());
    exit(SDL_ERROR);
}

void setup_graphics(void)
{
    w = SDL_CreateWindow("PONG", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (w == NULL)
        on_sdl_error();

    r = SDL_CreateRenderer(w, -1, SDL_RENDERER_PRESENTVSYNC);
    if (r == NULL)
        on_sdl_error();

    if (SDL_RenderSetLogicalSize(r, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
        on_sdl_error();

    if (SDL_SetWindowFullscreen(w, SDL_WINDOW_FULLSCREEN))
        on_sdl_error();

    SDL_SetRenderDrawColor(r, BACKGROUND_COLOR);
    SDL_RenderClear(r);
    SDL_RenderPresent(r);
}

void setup_paddles(void)
{
    player.h = PADDLE_HEIGHT;
    player.w = PADDLE_WIDTH;
    player.x = 0 + SCREEN_HORIZONTAL_GAP;
    player.y = SCREEN_HEIGHT / 2;

    computer.h = PADDLE_HEIGHT;
    computer.w = PADDLE_WIDTH;
    computer.x = SCREEN_WIDTH - SCREEN_HORIZONTAL_GAP - PADDLE_WIDTH;
    computer.y = SCREEN_HEIGHT / 2;
}

void render(void)
{
    SDL_SetRenderDrawColor(r, BACKGROUND_COLOR);
    SDL_RenderClear(r);

    SDL_SetRenderDrawColor(r, SPRITE_COLOR);
    SDL_RenderFillRect(r, &player);
    SDL_RenderFillRect(r, &computer);

    SDL_RenderPresent(r);
}

void handle_input(void)
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_KEYDOWN:
            switch (e.key.keysym.scancode)
            {
            case SDL_SCANCODE_MINUS:
                running = 0;
                break;
            case SDL_SCANCODE_W:
                player.y -= PADDLE_SPEED;
                if (player.y < 0 + SCREEN_VERTICAL_GAP)
                    player.y = 0 + SCREEN_VERTICAL_GAP;
                break;
            case SDL_SCANCODE_UP:
                player.y -= PADDLE_SPEED;
                if (player.y < 0 + SCREEN_VERTICAL_GAP)
                    player.y = 0 + SCREEN_VERTICAL_GAP;
                break;
            case SDL_SCANCODE_S:
                player.y += PADDLE_SPEED;
                if (player.y > SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - PADDLE_HEIGHT)
                    player.y = SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - PADDLE_HEIGHT;
                break;
            case SDL_SCANCODE_DOWN:
                player.y += PADDLE_SPEED;
                if (player.y > SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - PADDLE_HEIGHT)
                    player.y = SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - PADDLE_HEIGHT;
                break;
            }
            break;
        default:
            break;
        }
    }
}