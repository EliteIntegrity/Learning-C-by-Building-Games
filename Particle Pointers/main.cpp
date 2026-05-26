// ============================================================
//  Particle Fountain  -  Chapter 13 project
// ============================================================
//
//  The Click Particle program from Chapter 11, extended to
//  hold MANY particles at once using std::vector<Particle*>.
//  Every left-click spawns a new particle on the heap and
//  appends its address to the vector. Particles bounce around
//  the walls forever; the only limit is your patience and
//  available memory.
//
//  This is the project from Chapter 13 of "Learning C++ by
//  Building Games". It picks up directly from Chapter 11's
//  single-particle "Click Particle" project (sibling folder
//  of the same name) and lifts the one-at-a-time restriction
//  using the std::vector covered in Chapter 12.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

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

    SDL_Window* window = SDL_CreateWindow("Particle Fountain", WINDOW_W, WINDOW_H, 0);
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

    // The whole game's state. The vector starts empty.
    // reserve() asks the vector to allocate space up-front so the first
    // few hundred push_backs don't cause repeated reallocation. Pure
    // performance hint — does not change behavior.
    std::vector<Particle*> particles;
    particles.reserve(1024);

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
                // Allocate a new Particle on the heap and append its
                // address to our vector. The vector now owns the pointer.
                Particle* p = new Particle();

                p->x = event.button.x;
                p->y = event.button.y;

                float angle = randFloat(0.0f, 6.2832f);
                float speed = randFloat(MIN_SPEED, MAX_SPEED);
                p->vx = cosf(angle) * speed;
                p->vy = sinf(angle) * speed;

                p->size = randFloat(MIN_SIZE, MAX_SIZE);

                p->r = (Uint8)(rand() % 156 + 100);
                p->g = (Uint8)(rand() % 156 + 100);
                p->b = (Uint8)(rand() % 156 + 100);

                particles.push_back(p);
            }
        }

        Uint64 now = SDL_GetTicks();
        float  dt  = (now - lastTime) / 1000.0f;
        lastTime   = now;
        if (dt > 0.05f) dt = 0.05f;

        // Update every particle. The range-based for hands us each
        // pointer in turn; the loop body is the same logic as in
        // Chapter 11, applied to every particle instead of one.
        for (Particle* p : particles)
        {
            p->x += p->vx * dt;
            p->y += p->vy * dt;

            if (p->x < 0.0f)
            {
                p->x  = 0.0f;
                p->vx = -p->vx;
            }
            else if (p->x + p->size > WINDOW_W)
            {
                p->x  = WINDOW_W - p->size;
                p->vx = -p->vx;
            }

            if (p->y < 0.0f)
            {
                p->y  = 0.0f;
                p->vy = -p->vy;
            }
            else if (p->y + p->size > WINDOW_H)
            {
                p->y  = WINDOW_H - p->size;
                p->vy = -p->vy;
            }
        }

        SDL_SetRenderDrawColor(renderer, 12, 12, 24, 255);
        SDL_RenderClear(renderer);

        for (Particle* p : particles)
        {
            SDL_SetRenderDrawColor(renderer, p->r, p->g, p->b, 255);
            SDL_FRect rect = { p->x, p->y, p->size, p->size };
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    // Every Particle in the vector came from `new`, so every one needs
    // a matching `delete`. Walk the vector, free each, then clear it.
    for (Particle* p : particles)
        delete p;
    particles.clear();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
