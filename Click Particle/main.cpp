// ============================================================
//  Click Particle  -  Chapter 11 project
// ============================================================
//
//  A single colored square lives on the heap. Left-click
//  anywhere in the window: the previous square (if any) is
//  deleted, a new one is allocated with `new Particle()` at
//  the click position, and it flies off bouncing around the
//  walls forever.
//
//  This is the project from Chapter 11 of "Learning C++ by
//  Building Games". It is deliberately limited to one
//  particle at a time so the focus stays on raw pointers,
//  `new`, `delete`, `nullptr`, and the arrow operator. The
//  Chapter 13 project ("Particle Pointers" folder) lifts the
//  one-particle limit using std::vector.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int   WINDOW_W  = 800;
const int   WINDOW_H  = 600;
const float MIN_SPEED = 180.0f;
const float MAX_SPEED = 360.0f;
const float MIN_SIZE  = 12.0f;
const float MAX_SIZE  = 24.0f;

struct Particle
{
    float x, y;
    float vx, vy;
    float size;
    Uint8 r, g, b;
};

float randFloat(float lo, float hi)
{
    return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
}

int main(int argc, char* argv[])
{
    srand((unsigned)time(nullptr));

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Click Particle", WINDOW_W, WINDOW_H, 0);
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

    // The entire game's state: one pointer, initially pointing at nothing.
    Particle* particle = nullptr;

    Uint64 lastTime = SDL_GetTicks();

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

            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                // 1. Destroy the old particle, if there was one.
                delete particle;
                particle = nullptr;

                // 2. Allocate a brand new Particle on the heap.
                particle = new Particle();

                // 3. Fill in its fields.
                particle->x = event.button.x;
                particle->y = event.button.y;

                float angle = randFloat(0.0f, 6.2832f);
                float speed = randFloat(MIN_SPEED, MAX_SPEED);
                particle->vx = cosf(angle) * speed;
                particle->vy = sinf(angle) * speed;

                particle->size = randFloat(MIN_SIZE, MAX_SIZE);

                particle->r = (Uint8)(rand() % 156 + 100);
                particle->g = (Uint8)(rand() % 156 + 100);
                particle->b = (Uint8)(rand() % 156 + 100);
            }
        }

        Uint64 now = SDL_GetTicks();
        float  dt  = (now - lastTime) / 1000.0f;
        lastTime   = now;
        if (dt > 0.05f) dt = 0.05f;

        // Always guard a raw pointer before dereferencing it.
        if (particle != nullptr)
        {
            particle->x += particle->vx * dt;
            particle->y += particle->vy * dt;

            if (particle->x < 0.0f)
            {
                particle->x  = 0.0f;
                particle->vx = -particle->vx;
            }
            else if (particle->x + particle->size > WINDOW_W)
            {
                particle->x  = WINDOW_W - particle->size;
                particle->vx = -particle->vx;
            }

            if (particle->y < 0.0f)
            {
                particle->y  = 0.0f;
                particle->vy = -particle->vy;
            }
            else if (particle->y + particle->size > WINDOW_H)
            {
                particle->y  = WINDOW_H - particle->size;
                particle->vy = -particle->vy;
            }
        }

        SDL_SetRenderDrawColor(renderer, 12, 12, 24, 255);
        SDL_RenderClear(renderer);

        if (particle != nullptr)
        {
            SDL_SetRenderDrawColor(renderer,
                                   particle->r, particle->g, particle->b, 255);
            SDL_FRect rect = {
                particle->x, particle->y,
                particle->size, particle->size
            };
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    // The user has quit. If a particle is still alive on the heap,
    // we own it, so we have to free it before exiting.
    delete particle;
    particle = nullptr;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
