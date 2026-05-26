#include <SDL3/SDL.h>
#include <SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <memory>
#include <vector>

#include "Player.h"
#include "Enemy.h"
#include "HUD.h"

/* ============================================================================
 *  SDL3 Animated Character II  -  Inheritance & Polymorphism
 * ============================================================================
 *
 *  WHAT THIS PROGRAM DOES
 *    Pure-black fullscreen window. A normal Player runs in place at centre
 *    screen. Three Enemies (the same sprite, each tinted a different
 *    colour and flipped horizontally) move leftwards across the screen
 *    at different speeds, wrapping around the edges. The HUD shows a
 *    smoothed FPS counter. Press ESC (or close the window) to quit.
 *
 *
 *  WHAT IT TEACHES (the new ideas, on top of the previous project)
 *
 *    1. INHERITANCE  ("is-a")
 *       Both Player and Enemy IS-A Entity. Shared code (animator,
 *       position, basic update / render) lives in the Entity base
 *       class. Each derived class adds or overrides only what it
 *       needs to differ.   --> See Entity.h
 *
 *    2. VIRTUAL FUNCTIONS
 *       Entity::update and Entity::render are declared `virtual`.
 *       That signals "subclasses may replace me." Without `virtual`,
 *       calling update() through an Entity* pointer would always run
 *       the Entity version even if the object was really an Enemy.
 *       --> See Entity.h
 *
 *    3. OVERRIDE
 *       Enemy uses the `override` keyword on its update and render.
 *       It's optional, but it lets the compiler catch typos and
 *       signature mismatches that would otherwise create silent bugs.
 *       --> See Enemy.h
 *
 *    4. VIRTUAL DESTRUCTOR
 *       The single most important rule when designing a polymorphic
 *       base class: its destructor must be virtual. Without it,
 *       `delete entityPointer` only runs the base destructor and any
 *       resources owned by the derived part leak. We use
 *       `virtual ~Entity() = default;`.   --> See Entity.h
 *
 *    5. POLYMORPHISM  (the punchline)
 *       Look at the main loop below. We hold a
 *
 *           std::vector<std::unique_ptr<Entity>> entities;
 *
 *       containing a Player and three Enemies. The loop just calls
 *       entity->update(dt) and entity->render(renderer). The same
 *       lines of code drive completely different behaviour - Player
 *       gets the inherited defaults, Enemy gets its overrides - and
 *       the loop neither knows nor cares which is which. THAT is
 *       polymorphism: one piece of code, many concrete behaviours.
 *
 *
 *  RECOMMENDED READ ORDER
 *       Animator.h  ->  Entity.h  ->  Player.h  ->  Enemy.h  ->  HUD.h
 *       ->  main.cpp
 *
 *
 *  NOTE ON std::unique_ptr
 *
 *  Why `std::unique_ptr<Entity>` instead of plain `Entity*`? unique_ptr
 *  is a small wrapper around a raw pointer that automatically calls
 *  delete when it goes out of scope. Putting unique_ptr<Entity> in a
 *  vector means the vector owns the entities; when the vector is
 *  destroyed at the end of main, every Entity it holds is deleted -
 *  and because Entity has a virtual destructor (see point 4), the
 *  CORRECT derived destructor runs for each one. Zero `delete`
 *  statements anywhere. RAII strikes again.
 * ========================================================================== */

int main(int /*argc*/, char* /*argv*/[]) {

    // ------------------------------------------------------------------
    // 1. SDL initialization  (same as before)
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
        "Animated Character II - Inheritance & Polymorphism",
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
    //
    //    Each character will own its OWN copy of the sprite texture. We
    //    therefore need to load the texture once per character so each
    //    can be tinted independently without affecting the others.
    //    A small helper lambda makes that tidy.
    // ------------------------------------------------------------------
    auto loadSheet = [&]() -> SDL_Texture* {
        SDL_Texture* t = IMG_LoadTexture(renderer, "sprite_sheet.png");
        if (t) SDL_SetTextureScaleMode(t, SDL_SCALEMODE_LINEAR);
        return t;
    };

    SDL_Texture* playerSheet  = loadSheet();
    SDL_Texture* enemySheet1  = loadSheet();
    SDL_Texture* enemySheet2  = loadSheet();
    SDL_Texture* enemySheet3  = loadSheet();
    TTF_Font*    font         = TTF_OpenFont("RobotoMono-Light.ttf", 28);

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
        // ---- THE POLYMORPHIC CONTAINER ------------------------------
        // The vector holds pointers to the BASE class. At runtime each
        // slot may actually contain a Player or an Enemy - the vector
        // doesn't know or care. unique_ptr handles deletion for us.
        std::vector<std::unique_ptr<Entity>> entities;

        // Helper for placing enemies vertically.
        auto enemyY = [&](float scale) {
            const float h = screenH * scale;
            return screenH - h - screenH * 0.05f;
        };

        // Aspect of one frame, used to derive a width from a height.
        constexpr float kFrameAspect = (2288.0f / 6.0f) / 456.0f;

        // The Player goes in first so it draws BEHIND the enemies.
        entities.push_back(std::make_unique<Player>(
            playerSheet, /*frames=*/6, /*frameDur=*/1.0f / 8.0f,
            static_cast<float>(screenW), static_cast<float>(screenH)));

        // Three enemies, each at a different scale / speed / tint.
        // Notice we add them through Entity pointers - the vector
        // never sees the concrete Enemy type after this line.
        struct EnemySpec { float scale, speed; SDL_Color tint; };
        const EnemySpec specs[] = {
            { 0.55f, 220.0f, SDL_Color{ 255, 110, 110, 255 } }, // red-ish
            { 0.45f, 320.0f, SDL_Color{ 110, 220, 130, 255 } }, // green-ish
            { 0.35f, 420.0f, SDL_Color{ 110, 170, 255, 255 } }, // blue-ish
        };

        SDL_Texture* enemySheets[] = { enemySheet1, enemySheet2, enemySheet3 };
        for (int i = 0; i < 3; ++i) {
            const float h = screenH * specs[i].scale;
            const float w = h * kFrameAspect;
            const float x = static_cast<float>(screenW)
                          + i * (static_cast<float>(screenW) / 3.0f);
            entities.push_back(std::make_unique<Enemy>(
                enemySheets[i], /*frames=*/6, /*frameDur=*/1.0f / 8.0f,
                x, enemyY(specs[i].scale),
                w, h,
                specs[i].speed,
                static_cast<float>(screenW),
                specs[i].tint));
        }

        HUD hud(font, renderer);

        // ---- Main loop ---------------------------------------------
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

            // -------- THE POLYMORPHIC LOOP --------------------------
            // This is the moment everything pays off. One pair of
            // calls drives every entity in the world. Player runs
            // Entity's default update/render; each Enemy runs its
            // own overridden versions. The loop is identical either
            // way - we never check or cast or branch on type.
            for (auto& entity : entities) {
                entity->update(dt);
            }

            hud.update(dt);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);   // pure black
            SDL_RenderClear(renderer);

            for (const auto& entity : entities) {
                entity->render(renderer);
            }

            hud.render(renderer);
            SDL_RenderPresent(renderer);
        }

        // When this scope ends:
        //   - hud is destroyed   -> font closed
        //   - entities vector destroyed -> each unique_ptr destroyed
        //                                 -> ~Player / ~Enemy run
        //                                 -> animator destroyed
        //                                 -> SDL_DestroyTexture called
        // Because Entity's destructor is virtual, the CORRECT derived
        // destructor runs for each entity. Forget that one keyword
        // and you get silent leaks. (We told you it was important.)
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
