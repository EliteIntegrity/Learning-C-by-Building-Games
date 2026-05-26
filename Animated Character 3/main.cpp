#include <SDL3/SDL.h>
#include <SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <memory>
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "HUD.h"
#include "IUpdatable.h"
#include "IDrawable.h"

/* ============================================================================
 *  SDL3 Animated Character III  -  Interfaces
 * ============================================================================
 *
 *  WHAT THIS PROGRAM DOES
 *    Same visual result as the previous project: pure-black fullscreen
 *    window, a centred Player, three tinted-and-flipped Enemies running
 *    leftward, and a smoothed FPS counter. The interesting work is in
 *    the source code, not on the screen.
 *
 *
 *  WHAT IT TEACHES (the new ideas, on top of inheritance & polymorphism)
 *
 *    1. AN INTERFACE IS A CONTRACT WITH NO STATE
 *       IUpdatable and IDrawable are pure-virtual classes with no
 *       data members. They define WHAT a class promises to do, not
 *       how it does it.   --> See IUpdatable.h
 *
 *    2. UNRELATED CLASSES CAN SHARE AN INTERFACE
 *       Entity (a base for game characters) and HUD (a UI widget)
 *       have nothing structurally in common. Yet both implement
 *       IUpdatable and IDrawable, so both can be driven by the
 *       same loop.   --> See HUD.h
 *
 *    3. MULTIPLE INHERITANCE IS SAFE WITH INTERFACES
 *       Entity and HUD each inherit from BOTH IUpdatable and
 *       IDrawable. C++ multiple inheritance is famously dangerous
 *       with stateful bases, but inheriting from many INTERFACES is
 *       routine and safe.   --> See Entity.h
 *
 *    4. POLYMORPHISM ACROSS UNRELATED TYPES
 *       Look at the two `for` loops in the main loop below. They
 *       walk a `std::vector<IUpdatable*>` and a
 *       `std::vector<IDrawable*>`. The vectors hold pointers to a
 *       Player, three Enemies, and a HUD - completely different
 *       concrete types. The loop calls the same method on every
 *       one of them and dispatches correctly via virtual call.
 *       This is the most far-reaching form of polymorphism: same
 *       code drives types that share NOTHING except the contract.
 *
 *    5. OPEN/CLOSED PRINCIPLE
 *       Adding a new updatable / drawable thing later (a particle
 *       system, a debug overlay, a background) requires no change
 *       to main.cpp. Just write a new class that implements the
 *       interfaces and push it into the lists. The existing code
 *       is "open for extension, closed for modification."
 *
 *
 *  RECOMMENDED READ ORDER
 *       IUpdatable.h  ->  IDrawable.h  ->  Animator.h  ->  Entity.h
 *       ->  Player.h  ->  Enemy.h  ->  HUD.h  ->  main.cpp
 *
 *
 *  NOTE ON OWNERSHIP
 *
 *  Two ownership patterns appear side by side here, and it's worth
 *  understanding why:
 *
 *    - The `entities` vector holds  std::unique_ptr<Entity>.  It
 *      OWNS the Player and Enemies. When it goes out of scope, the
 *      objects are deleted (and their virtual destructors run, freeing
 *      textures).
 *
 *    - The `updatables` and `drawables` vectors hold RAW POINTERS
 *      (IUpdatable* / IDrawable*). They're NON-OWNING - just two
 *      different views of the same underlying objects. The single
 *      source of truth for ownership is `entities` plus the local
 *      `hud` variable.
 *
 *  This split is normal and healthy. One pointer should own; many
 *  pointers may observe.
 * ========================================================================== */

int main(int /*argc*/, char* /*argv*/[]) {

    // ------------------------------------------------------------------
    // 1. SDL initialization
    // ------------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    const SDL_DisplayID    display = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode    = SDL_GetDesktopDisplayMode(display);
    const int screenW = mode ? mode->w : 1920;
    const int screenH = mode ? mode->h : 1080;

    SDL_Window* window = SDL_CreateWindow(
        "Animated Character III - Interfaces",
        screenW, screenH,
        SDL_WINDOW_FULLSCREEN);
    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window); TTF_Quit(); SDL_Quit();
        return 1;
    }
    SDL_SetRenderVSync(renderer, 1);

    // ------------------------------------------------------------------
    // 2. Load assets
    // ------------------------------------------------------------------
    auto loadSheet = [&]() -> SDL_Texture* {
        SDL_Texture* t = IMG_LoadTexture(renderer, "sprite_sheet.png");
        if (t) SDL_SetTextureScaleMode(t, SDL_SCALEMODE_LINEAR);
        return t;
    };

    SDL_Texture* playerSheet = loadSheet();
    SDL_Texture* enemySheet1 = loadSheet();
    SDL_Texture* enemySheet2 = loadSheet();
    SDL_Texture* enemySheet3 = loadSheet();
    TTF_Font*    font        = TTF_OpenFont("RobotoMono-Light.ttf", 28);

    if (!playerSheet || !enemySheet1 || !enemySheet2 || !enemySheet3 || !font) {
        SDL_Log("Asset load failed: %s", SDL_GetError());
        if (playerSheet) SDL_DestroyTexture(playerSheet);
        if (enemySheet1) SDL_DestroyTexture(enemySheet1);
        if (enemySheet2) SDL_DestroyTexture(enemySheet2);
        if (enemySheet3) SDL_DestroyTexture(enemySheet3);
        if (font)        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    // ------------------------------------------------------------------
    // 3. Build the world inside a scope block, run, then let go.
    // ------------------------------------------------------------------
    {
        // ---- The OWNING container of game characters --------------
        std::vector<std::unique_ptr<Entity>> entities;

        constexpr float kFrameAspect = (2288.0f / 6.0f) / 456.0f;
        auto enemyY = [&](float scale) {
            const float h = screenH * scale;
            return screenH - h - screenH * 0.05f;
        };

        entities.push_back(std::make_unique<Player>(
            playerSheet, /*frames=*/6, /*frameDur=*/1.0f / 8.0f,
            static_cast<float>(screenW), static_cast<float>(screenH)));

        struct EnemySpec { float scale, speed; SDL_Color tint; };
        const EnemySpec specs[] = {
            { 0.55f, 220.0f, SDL_Color{ 255, 110, 110, 255 } },
            { 0.45f, 320.0f, SDL_Color{ 110, 220, 130, 255 } },
            { 0.35f, 420.0f, SDL_Color{ 110, 170, 255, 255 } },
        };
        SDL_Texture* enemySheets[] = { enemySheet1, enemySheet2, enemySheet3 };
        for (int i = 0; i < 3; ++i) {
            const float h = screenH * specs[i].scale;
            const float w = h * kFrameAspect;
            const float x = static_cast<float>(screenW)
                          + i * (static_cast<float>(screenW) / 3.0f);
            entities.push_back(std::make_unique<Enemy>(
                enemySheets[i], 6, 1.0f / 8.0f,
                x, enemyY(specs[i].scale),
                w, h,
                specs[i].speed,
                static_cast<float>(screenW),
                specs[i].tint));
        }

        HUD hud(font, renderer);

        // ---- THE INTERFACE LISTS  (the punchline) -----------------
        //
        // We now build TWO lists of NON-OWNING interface pointers:
        //
        //   updatables - everything that wants update(dt) called
        //   drawables  - everything that wants render(renderer) called
        //
        // Each list contains a mix of types that share NO base class
        // except the interface itself: Player, Enemy, and HUD all sit
        // happily side by side. The C++ compiler quietly UPCASTS each
        // raw pointer (Entity*, HUD*) to the interface type when it's
        // pushed in - that's automatic because Entity and HUD both
        // publicly inherit from the interfaces.
        std::vector<IUpdatable*> updatables;
        std::vector<IDrawable*>  drawables;

        for (auto& e : entities) {
            updatables.push_back(e.get());      // Entity* -> IUpdatable*
            drawables.push_back (e.get());      // Entity* -> IDrawable*
        }
        updatables.push_back(&hud);             // HUD*    -> IUpdatable*
        drawables.push_back (&hud);             // HUD*    -> IDrawable*

        // ---- Main loop --------------------------------------------
        bool   running   = true;
        Uint64 lastTicks = SDL_GetTicksNS();

        while (running) {

            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_EVENT_QUIT) running = false;
                if (ev.type == SDL_EVENT_KEY_DOWN &&
                    ev.key.scancode == SDL_SCANCODE_ESCAPE) running = false;
            }

            const Uint64 now = SDL_GetTicksNS();
            const float  dt  = static_cast<float>(now - lastTicks) / 1.0e9f;
            lastTicks = now;

            // -------- ONE LOOP UPDATES EVERYTHING ------------------
            // Player, three Enemies, and HUD - tick'd by the same
            // line of code. The loop has no idea what types those
            // pointers really refer to, and doesn't need to.
            for (auto* u : updatables) {
                u->update(dt);
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // -------- ONE LOOP DRAWS EVERYTHING --------------------
            // Insertion order = draw order, so entities appear
            // behind the HUD (HUD was pushed last).
            for (const auto* d : drawables) {
                d->render(renderer);
            }

            SDL_RenderPresent(renderer);
        }

        // When this scope ends:
        //   - updatables / drawables vectors discarded (no ownership)
        //   - hud destroyed -> font closed
        //   - entities destroyed -> each Player/Enemy destructor runs
        //                          (correctly, because the destructors
        //                          inherited their virtuality from the
        //                          interfaces - no manual `virtual ~`
        //                          needed in Entity).
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
