// ============================================================
//  Animated Character (Interfaces)  -  Chapter 21 project
//  Act 3 Capstone
// ============================================================
//
//  The Chapter 19 inheritance project, redone now that we
//  have virtual functions, virtual destructors, and pure-
//  virtual interfaces from Chapter 20.
//
//  THE TWO PAYOFFS
//
//  1. One polymorphic vector of entities. We store a Player
//     and three Enemies in:
//
//         std::vector<std::unique_ptr<Entity>> entities;
//
//     One update loop ticks every one of them. Player runs
//     Entity::update; each Enemy runs its overridden version.
//     The same line of code drives both. Compare to Chapter
//     19, which needed two separate typed containers.
//
//  2. HUD joins the polymorphic loops via interfaces. HUD is
//     not an Entity and never will be — it has no Animator,
//     no position. But it implements IUpdatable and IDrawable
//     just like Entity does, so it can be added to two
//     interface-typed views (one for updating, one for
//     drawing) that include both entities and HUD without
//     either side knowing about the other's concrete type.
//
//  This is the in-book project from Chapter 21 of "Learning
//  C++ by Building Games" and the Act 3 capstone. A richer
//  asset-driven version of the same architecture (sprite
//  sheet, TTF font, multiple sheets per enemy for tinting)
//  lives in the sibling "Animated Character 3" folder.
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
        "Animated Character - Interfaces",
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
        // ---- Owning storage --------------------------------------------
        // unique_ptr owns each entity; the vector owns the unique_ptrs.
        // When this block ends, every entity is destroyed in order via
        // the virtual destructor in IUpdatable / IDrawable (which
        // propagates through Entity).
        std::vector<std::unique_ptr<Entity>> entities;
        HUD hud;

        // Player first so it draws BEHIND the enemies.
        entities.push_back(std::make_unique<Player>(
            (float)WINDOW_W, (float)WINDOW_H));

        struct Spec { float w, h, y, speed; SDL_Color color; };
        const Spec specs[] = {
            {  80.0f,  80.0f, 100.0f, 120.0f, { 220, 100, 100, 255 } },
            {  60.0f,  60.0f, 260.0f, 220.0f, { 100, 220, 130, 255 } },
            {  40.0f,  40.0f, 420.0f, 320.0f, { 100, 170, 255, 255 } },
        };

        for (int i = 0; i < 3; ++i)
        {
            const float startX = (float)WINDOW_W + i * 200.0f;
            entities.push_back(std::make_unique<Enemy>(
                startX, specs[i].y,
                specs[i].w, specs[i].h,
                specs[i].speed,
                (float)WINDOW_W,
                specs[i].color));
        }

        // ---- Non-owning interface views --------------------------------
        // These vectors hold RAW pointers. They don't own anything — the
        // unique_ptrs and the `hud` variable above are the owners. These
        // are just "ways to look at" everything that implements an
        // interface so the main loop can treat them uniformly.
        std::vector<IUpdatable*> updatables;
        std::vector<IDrawable*>  drawables;

        for (auto& e : entities)
        {
            updatables.push_back(e.get());
            drawables.push_back(e.get());
        }
        updatables.push_back(&hud);
        drawables.push_back(&hud);

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

            // -- The single polymorphic update loop --
            // Every updatable (every entity AND the HUD) ticks here.
            for (auto* u : updatables)
                u->update(dt);

            // -- The single polymorphic render loop --
            SDL_SetRenderDrawColor(renderer, 30, 35, 45, 255);
            SDL_RenderClear(renderer);

            for (const auto* d : drawables)
                d->render(renderer);

            SDL_RenderPresent(renderer);
        }
    }
    // <-- entities and hud are destroyed HERE. Because every
    //     destructor in the chain is virtual (inherited from the
    //     interfaces), the correct derived dtor runs for each.

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
