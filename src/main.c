#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "embedded_font.h"
#include "embedded_licenses.h"

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

#define COMPUTER_PADDLE_DELAY 2

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
int smoke_test(void);

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

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "--licenses") == 0)
    {
        puts((const char *)embedded_licenses);
        return 0;
    }
    SDL_SetMainReady();
    setup_graphics();
    setup_fonts();
    int status = 0;
    if (argc > 1 && strcmp(argv[1], "--smoke-test") == 0)
    {
        status = smoke_test();
        goto cleanup;
    }

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
        if (!running)
            break;
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
        SDL_Delay(10);
    }

cleanup:
    TTF_CloseFont(f);
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    TTF_Quit();
    SDL_Quit();
    return status;
}

void on_sdl_error(void)
{
    fprintf(stderr, "Pong error: %s\n", SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Pong", SDL_GetError(), w);
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
        r = SDL_CreateRenderer(w, -1, SDL_RENDERER_SOFTWARE);
    if (r == NULL)
        on_sdl_error();

    if (SDL_RenderSetLogicalSize(r, SCREEN_WIDTH, SCREEN_HEIGHT) < 0)
        on_sdl_error();

    if (SDL_SetWindowFullscreen(w, SDL_WINDOW_FULLSCREEN_DESKTOP))
        on_sdl_error();

    SDL_SetRenderDrawColor(r, BACKGROUND_COLOR);
    SDL_RenderClear(r);
    SDL_RenderPresent(r);
}

void setup_fonts(void)
{
    SDL_RWops *font_data = SDL_RWFromConstMem(embedded_font, (int)sizeof(embedded_font));
    if (font_data == NULL)
        on_sdl_error();
    f = TTF_OpenFontRW(font_data, 1, 256);
    if (f == NULL)
        on_sdl_error();
}

void reset_variables(void)
{
    running = 0;
    wait_input = 0;
    quit = 0;
    restart = 0;
    ball_speed = BALL_INITIAL_SPEED;
    computer_delay = COMPUTER_PADDLE_DELAY;
    increase_ball_speed_interval = INCREASE_BALL_SPEED_INTERVAL;
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
        case SDL_QUIT:
            running = 0;
            wait_input = 0;
            quit = 1;
            break;
        case SDL_KEYDOWN:
            switch (e.key.keysym.scancode)
            {
            case SDL_SCANCODE_MINUS:
            case SDL_SCANCODE_ESCAPE:
                running = 0;
                wait_input = 0;
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
            default:
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
    if (result_surface == NULL)
        on_sdl_error();
    SDL_Texture *result_texture = SDL_CreateTextureFromSurface(r, result_surface);
    SDL_FreeSurface(result_surface);
    if (result_texture == NULL)
        on_sdl_error();

    SDL_Surface *restart_surface = TTF_RenderText_Blended(f, restart_message, text_color);
    if (restart_surface == NULL)
        on_sdl_error();
    SDL_Texture *restart_texture = SDL_CreateTextureFromSurface(r, restart_surface);
    SDL_FreeSurface(restart_surface);
    if (restart_texture == NULL)
        on_sdl_error();

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
    SDL_DestroyTexture(result_texture);
    SDL_DestroyTexture(restart_texture);
}

/* Exercises rendering and input with only the executable present. */
int smoke_test(void)
{
    reset_variables();
    setup_paddles();
    setup_ball();
    start_game();
    render();
    move_ball();
    ball_direction_x = 1;
    int computer_previous_y = computer.y;
    move_computer_paddle();
    if (computer.y != computer_previous_y || computer_delay != 1)
        return 1;
    move_computer_paddle();
    if (computer.y != computer_previous_y - PADDLE_SPEED ||
        computer_delay != COMPUTER_PADDLE_DELAY)
        return 1;
    check_collision();
    show_game_result(VICTORY);
    show_game_result(LOSE);
    SDL_Event event = {0};
    event.type = SDL_KEYDOWN;
    event.key.keysym.scancode = SDL_SCANCODE_R;
    if (SDL_PushEvent(&event) < 0)
        return 1;
    handle_input();
    if (!restart)
        return 1;
    reset_variables();
    if (wait_input || restart || quit || ball_speed != BALL_INITIAL_SPEED)
        return 1;
    start_game();
    event.key.keysym.scancode = SDL_SCANCODE_W;
    int previous_y = player.y;
    if (SDL_PushEvent(&event) < 0)
        return 1;
    handle_input();
    if (player.y != previous_y - PADDLE_SPEED)
        return 1;
    event.key.keysym.scancode = SDL_SCANCODE_ESCAPE;
    if (SDL_PushEvent(&event) < 0)
        return 1;
    handle_input();
    if (running || wait_input || !quit)
        return 1;
    reset_variables();
    start_game();
    event.type = SDL_QUIT;
    if (SDL_PushEvent(&event) < 0)
        return 1;
    handle_input();
    return running || wait_input || !quit;
}
