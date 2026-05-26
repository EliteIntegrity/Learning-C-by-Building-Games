// ============================================================
//  Controllable Square  -  Chapter 1 project
// ============================================================
//
//  An 800x600 window with a colored square in the middle.
//  WASD moves the square, Escape (or window-X) quits.
//
//  This is the project from Chapter 1 of "Learning C++ by
//  Building Games". Every line is explained step by step in
//  the chapter.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W = 800;
const int   WINDOW_H = 600;
const float SQUARE_SZ = 80.0f;
const float SPEED = 300.0f;

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Controllable Square",
        WINDOW_W, WINDOW_H,
        0
    );
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

    float x = (WINDOW_W - SQUARE_SZ) / 2.0f;
    float y = (WINDOW_H - SQUARE_SZ) / 2.0f;

    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        // -- Events --
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
                running = false;
        }

        // -- Delta time --
        Uint64 now = SDL_GetTicks();
        float  delta = (now - lastTime) / 1000.0f;
        lastTime = now;

        // -- Input --
        const bool* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_W]) y -= SPEED * delta;
        if (keys[SDL_SCANCODE_S]) y += SPEED * delta;
        if (keys[SDL_SCANCODE_A]) x -= SPEED * delta;
        if (keys[SDL_SCANCODE_D]) x += SPEED * delta;

        // -- Keep on screen --
        x = SDL_clamp(x, 0.0f, WINDOW_W - SQUARE_SZ);
        y = SDL_clamp(y, 0.0f, WINDOW_H - SQUARE_SZ);

        // -- Render --
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
        SDL_FRect square = { x, y, SQUARE_SZ, SQUARE_SZ };
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
