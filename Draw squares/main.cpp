// ============================================================
//  Cascade  -  Chapter 7 project
// ============================================================
//
//  A diagonal cascade of colored squares draws from the
//  top-left of an 800x600 window toward the bottom-right,
//  cycling through a fixed 9-color palette. A small amount of
//  delta-time math shifts the color offset every 100 ms so
//  the rainbow appears to flow along the diagonal.
//
//  This is the project from Chapter 7 of "Learning C++ by
//  Building Games". It demonstrates how a single `for` loop
//  scales a few lines of code into a screen full of shapes.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int SCREEN_W  = 800;
const int SCREEN_H  = 600;
const int SQUARE_SZ = 30;
const int PADDING   = 2;
const int STEP      = SQUARE_SZ + PADDING;   // distance between square corners

const SDL_Color COLORS[] = {
    {220,  50,  50, 255},   // red
    {230, 140,  30, 255},   // orange
    {220, 210,  40, 255},   // yellow
    { 50, 180,  50, 255},   // green
    { 40, 140, 220, 255},   // blue
    {100,  60, 200, 255},   // indigo
    {170,  60, 200, 255},   // violet
    { 50, 200, 180, 255},   // teal
    {200,  80, 130, 255},   // pink
};
const int COLOR_COUNT = sizeof(COLORS) / sizeof(COLORS[0]);

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Cascade", SCREEN_W, SCREEN_H, 0);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Remember when we started, so we can offset the colors over time.
    Uint64 startTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        int x = PADDING;
        int y = PADDING;

        // One new color slot per 100 ms.
        int offset = (int)((SDL_GetTicks() - startTime) / 100);

        for (int i = 0; ; ++i)
        {
            // Stop the moment the next square wouldn't fit on either axis.
            if (x + SQUARE_SZ > SCREEN_W && y + SQUARE_SZ > SCREEN_H)
                break;

            SDL_Color c = COLORS[(i + offset) % COLOR_COUNT];
            SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);

            SDL_FRect rect = {
                (float)x, (float)y,
                (float)SQUARE_SZ, (float)SQUARE_SZ
            };
            SDL_RenderFillRect(renderer, &rect);

            x += STEP;
            y += STEP;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
