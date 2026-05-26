// ============================================================
//  Animated Character (Inheritance)  -  Chapter 19 project
// ============================================================
//
//  Builds on the Chapter 17 classes demo by introducing a
//  shared Entity base class. Both Player and Enemy derive
//  from Entity, sharing the Animator + position + render
//  logic without duplication. Player uses the base's behavior
//  unchanged; Enemy adds its own movement and color.
//
//  Note that we keep Player and Enemies in SEPARATELY-TYPED
//  containers — `Player player;` on its own, and a
//  `std::vector<std::unique_ptr<Enemy>>` for the enemies.
//  We do NOT mix them in a single `std::vector<Entity*>`
//  because, without `virtual`, the wrong methods would be
//  called. The polymorphism fix is in Chapter 20, and the
//  Chapter 21 project picks this code up and puts everything
//  into one container properly.
//
//  This is the in-book project from Chapter 19 of "Learning
//  C++ by Building Games". A richer asset-driven version
//  (which uses `virtual` and polymorphism, jumping ahead) is
//  in the sibling "Animated character 2 (Inheritance)" folder.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <memory>
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "HUD.h"

const int WINDOW_W = 1024;
const int WINDOW_H = 600;

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Animated Character - Inheritance",
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

    {
        // One Player on its own.
        Player player((float)WINDOW_W, (float)WINDOW_H);

        // Three Enemies in a vector typed to Enemy (not Entity!) so
        // every method call goes directly to Enemy's versions.
        std::vector<std::unique_ptr<Enemy>> enemies;

        struct Spec { float w, h, y, speed; SDL_Color color; };
        const Spec specs[] = {
            {  80.0f,  80.0f, 100.0f, 120.0f, { 220, 100, 100, 255 } }, // red
            {  60.0f,  60.0f, 260.0f, 220.0f, { 100, 220, 130, 255 } }, // green
            {  40.0f,  40.0f, 420.0f, 320.0f, { 100, 170, 255, 255 } }, // blue
        };

        for (int i = 0; i < 3; ++i)
        {
            const float startX = (float)WINDOW_W + i * 200.0f;
            enemies.push_back(std::make_unique<Enemy>(
                startX, specs[i].y,
                specs[i].w, specs[i].h,
                specs[i].speed,
                (float)WINDOW_W,
                specs[i].color));
        }

        HUD hud;

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

            // -- Update: player first, then enemies. Two loops, two
            //    concrete types, no polymorphism needed.
            player.update(dt);
            for (auto& e : enemies)
                e->update(dt);

            hud.update(dt);

            // -- Render: same shape.
            SDL_SetRenderDrawColor(renderer, 30, 35, 45, 255);
            SDL_RenderClear(renderer);

            player.render(renderer);
            for (const auto& e : enemies)
                e->render(renderer);

            hud.render(renderer);
            SDL_RenderPresent(renderer);
        }

        // When this block ends: enemies vector dies, each unique_ptr
        // destroys its Enemy. Player dies. HUD dies. Each one's
        // Animator dies, freeing its tints[] array. Standard RAII.
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
