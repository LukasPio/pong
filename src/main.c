#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SDL_ERROR 1

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_HORIZONTAL_GAP 20
#define SCREEN_VERTICAL_GAP 10

#define PADDLE_WIDTH 20
#define PADDLE_HEIGHT 100
#define PADDLE_SPEED 15

#define BALL_WIDTH 30
#define BALL_HEIGHT 30
#define BALL_INITIAL_SPEED 5
#define INCREASE_BALL_SPEED_INTERVAL 2

#define COMPUTER_PADDLE_DELAY 3

#define BACKGROUND_COLOR 0, 0, 0, SDL_ALPHA_OPAQUE
#define SPRITE_COLOR 255, 165, 0, SDL_ALPHA_OPAQUE

enum Result
{
    VICTORY,
    LOSE
};

void on_sdl_error(void);
void setup_graphics(void);
void setup_fonts(void);
void reset_variables(void);
void setup_paddles(void);
void setup_ball(void);
void start_game(void);
void render(void);
void handle_input(void);
void move_ball(void);
void move_computer_paddle(void);
void check_collision(void);
void show_game_result(enum Result result);

SDL_Window *w = NULL;
SDL_Renderer *r = NULL;
TTF_Font *f = NULL;

int running = 0;
int wait_input = 0;
int restart = 0;
int quit = 0;
int computer_delay = COMPUTER_PADDLE_DELAY;
int ball_speed = BALL_INITIAL_SPEED;
int increase_ball_speed_interval = INCREASE_BALL_SPEED_INTERVAL;

SDL_Rect player = {0};
SDL_Rect computer = {0};

SDL_Rect ball = {0};
int ball_direction_x, ball_direction_y;

int main()
{
    setup_graphics();
    setup_fonts();

restart:
    srand(time(NULL));
    reset_variables();

    setup_paddles();
    setup_ball();

    start_game();

    while (running)
    {
        render();
        handle_input();
        move_ball();
        move_computer_paddle();
        check_collision();
    }

    while (wait_input)
    {
        handle_input();
        if (restart)
        {
            goto restart;
        }
        if (quit)
            break;
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0 || TTF_Init() == -1)
        on_sdl_error();

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

void setup_fonts(void)
{
    f = TTF_OpenFont("./font/arial_font.ttf", 256);
    TTF_SetFontStyle(f, TTF_STYLE_BOLD);
}

void reset_variables(void)
{
    running = 0;
    quit = 0;
    restart = 0;
    ball_speed = BALL_INITIAL_SPEED;
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

void setup_ball(void)
{
    ball.h = BALL_HEIGHT;
    ball.w = BALL_WIDTH;
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_VERTICAL_GAP;

    ball_direction_x = rand() % 2 ? 1 : -1;
    ball_direction_y = 1;
}

void start_game(void)
{
    running = 1;
}

void render(void)
{
    SDL_SetRenderDrawColor(r, BACKGROUND_COLOR);
    SDL_RenderClear(r);

    SDL_SetRenderDrawColor(r, SPRITE_COLOR);

    SDL_RenderFillRect(r, &player);
    SDL_RenderFillRect(r, &computer);
    SDL_RenderFillRect(r, &ball);

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
                quit = 1;
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
            case SDL_SCANCODE_R:
                if (!running)
                    restart = 1;
                break;
            }
            break;
        default:
            break;
        }
    }
}

void move_ball(void)
{
    if (ball_direction_x == -1)
        ball.x -= ball_speed;
    else
        ball.x += ball_speed;

    if (ball_direction_y == -1)
        ball.y -= ball_speed;
    else
        ball.y += ball_speed;
}

void move_computer_paddle(void)
{
    if (ball_direction_x == -1)
        return;

    computer_delay--;
    if (computer_delay > 0)
        return;
    if (computer.y + PADDLE_HEIGHT / 2 > ball.y + BALL_HEIGHT / 2)
        computer.y -= PADDLE_SPEED;
    else
        computer.y += PADDLE_SPEED;

    if (computer.y > SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - PADDLE_HEIGHT)
        computer.y = SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - PADDLE_HEIGHT;

    if (computer.y < SCREEN_VERTICAL_GAP)
        computer.y = SCREEN_VERTICAL_GAP;

    computer_delay = COMPUTER_PADDLE_DELAY;
}

void check_collision(void)
{
    if (SDL_HasIntersection(&player, &ball) || SDL_HasIntersection(&computer, &ball))
    {
        increase_ball_speed_interval--;
        ball_direction_x *= -1;
        if (increase_ball_speed_interval == 0)
        {
            ball_speed++;
            increase_ball_speed_interval = INCREASE_BALL_SPEED_INTERVAL;
        }
    }

    if (ball.y <= SCREEN_VERTICAL_GAP)
        ball_direction_y *= -1;

    if (ball.y >= SCREEN_HEIGHT - SCREEN_VERTICAL_GAP - BALL_HEIGHT)
        ball_direction_y *= -1;

    if (ball.x <= 0)
        show_game_result(LOSE);

    if (ball.x >= SCREEN_WIDTH)
        show_game_result(VICTORY);
}

void show_game_result(enum Result result)
{
    running = 0;
    wait_input = 1;

    char *result_message = result == VICTORY ? "You won! Congratulations :)" : "You lose... Try again!";
    char *restart_message = "Press R to restart the game.";

    SDL_Color text_color = {SPRITE_COLOR};

    SDL_Surface *result_surface = TTF_RenderText_Blended(f, result_message, text_color);
    SDL_Texture *result_texture = SDL_CreateTextureFromSurface(r, result_surface);
    SDL_FreeSurface(result_surface);

    SDL_Surface *restart_surface = TTF_RenderText_Blended(f, restart_message, text_color);
    SDL_Texture *restart_texture = SDL_CreateTextureFromSurface(r, restart_surface);
    SDL_FreeSurface(restart_surface);

    SDL_Rect result_dst;
    result_dst.h = SCREEN_HEIGHT / 3;
    result_dst.w = SCREEN_WIDTH / 2;
    result_dst.x = (SCREEN_WIDTH - result_dst.w) / 2;
    result_dst.y = (SCREEN_HEIGHT - result_dst.h) / 2;

    SDL_Rect restart_dst;
    restart_dst.h = SCREEN_HEIGHT / 5;
    restart_dst.w = SCREEN_WIDTH / 3;
    restart_dst.x = result_dst.x + (result_dst.w - restart_dst.w) / 2;
    restart_dst.y = result_dst.y + restart_dst.h + 50;

    SDL_SetRenderDrawColor(r, BACKGROUND_COLOR);
    SDL_RenderClear(r);

    SDL_RenderCopy(r, result_texture, NULL, &result_dst);
    SDL_RenderCopy(r, restart_texture, NULL, &restart_dst);
    SDL_RenderPresent(r);
}