// ============================================================
//  Animated Character (Classes Demo)  -  Chapter 17 project
// ============================================================
//
//  An OOP refactor showcase. A Player object lives in the
//  middle of the window and renders as a rainbow-tinted
//  square; an Animator drives the tint cycle; a HUD draws a
//  smoothed FPS bar in the top-left.
//
//  This is the in-book project from Chapter 17 of "Learning
//  C++ by Building Games". A richer asset-driven version of
//  the same architecture (real sprite sheet, real TTF font)
//  lives in the sibling "Animated character" folder. The
//  class architecture is identical; only the rendering layer
//  differs.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Player.h"
#include "HUD.h"

const int WINDOW_W = 800;
const int WINDOW_H = 600;

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Animated Character - Classes Demo",
        WINDOW_W, WINDOW_H, 0);
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

    // Why this extra { } block?
    // We want `player` and `hud` to be destroyed BEFORE we tear
    // down the renderer and SDL itself. Wrapping them in their
    // own scope guarantees that order: when the block ends,
    // their destructors run; THEN we hit SDL_DestroyRenderer.
    {
        Player player((float)WINDOW_W, (float)WINDOW_H);
        HUD    hud;

        Uint64 lastTicks = SDL_GetTicksNS();
        bool   running   = true;

        while (running)
        {
            SDL_Event ev;
            while (SDL_PollEvent(&ev))
            {
                if (ev.type == SDL_EVENT_QUIT)
                    running = false;
                if (ev.type == SDL_EVENT_KEY_DOWN &&
                    ev.key.scancode == SDL_SCANCODE_ESCAPE)
                    running = false;
            }

            const Uint64 now = SDL_GetTicksNS();
            const float  dt  = (float)(now - lastTicks) / 1.0e9f;
            lastTicks = now;

            // The main loop's job is now embarrassingly small —
            // it just asks each object to update and render.
            player.update(dt);
            hud.update(dt);

            SDL_SetRenderDrawColor(renderer, 30, 35, 45, 255);
            SDL_RenderClear(renderer);

            player.render(renderer);
            hud.render(renderer);

            SDL_RenderPresent(renderer);
        }
    }
    // <-- player and hud are destroyed HERE, automatically.

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
