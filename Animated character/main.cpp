#include <SDL3/SDL.h>
#include <SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Player.h"
#include "HUD.h"

/* ============================================================================
 *  SDL3 Animated Character  -  A C++ Classes Demo
 * ============================================================================
 *
 *  WHAT THIS PROGRAM DOES
 *    Opens a fullscreen window, plays a 6-frame run cycle scaled to the
 *    screen, and shows a smoothed FPS counter in the top-left corner.
 *    Press ESC (or close the window) to quit.
 *
 *  WHAT IT TEACHES
 *
 *    1. CLASSES BUNDLE DATA + FUNCTIONS
 *       Each of our three classes (Animator, Player, HUD) packages up
 *       the data it needs and the operations that act on that data.
 *       Animator alone wouldn't be a useful object-oriented example -
 *       you really see the value when several classes interact.
 *
 *    2. ENCAPSULATION
 *       Every class keeps its data `private`. The rest of the program
 *       only ever calls `public` methods. This means each class can
 *       change how it works inside without breaking anything outside.
 *
 *    3. COMPOSITION ("HAS-A")
 *       Player HAS-A Animator (stored by value as a member). This is
 *       how most C++ objects relate to one another - much more often
 *       than inheritance, which we'll cover in the next project.
 *
 *    4. RAII (Resource Acquisition Is Initialization)
 *       Each class's constructor takes ownership of a resource (a
 *       texture, a font); each destructor releases it. Notice that
 *       main() never calls SDL_DestroyTexture or TTF_CloseFont -
 *       those happen automatically when `player` and `hud` go out
 *       of scope. Less code, fewer bugs.
 *
 *    5. DELEGATION
 *       The main loop is tiny. It just asks each object to update and
 *       render itself. That short, readable loop is the payoff of
 *       good class design.
 *
 *  RECOMMENDED READ ORDER
 *       Animator.h  ->  Player.h  ->  HUD.h  ->  main.cpp
 *
 *  Each header contains a short tutorial about the concept it shows.
 * ========================================================================== */

int main(int /*argc*/, char* /*argv*/[]) {

    // ------------------------------------------------------------------
    // 1. Initialize SDL and SDL_ttf.
    //    SDL3 functions return `true` on success, `false` on failure.
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

    // ------------------------------------------------------------------
    // 2. Find the desktop resolution and create a fullscreen window.
    // ------------------------------------------------------------------
    const SDL_DisplayID    display = SDL_GetPrimaryDisplay();
    const SDL_DisplayMode* mode    = SDL_GetDesktopDisplayMode(display);
    const int screenW = mode ? mode->w : 1920;
    const int screenH = mode ? mode->h : 1080;

    SDL_Window* window = SDL_CreateWindow(
        "Animated Character - Classes Demo",
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
    SDL_SetRenderVSync(renderer, 1);   // wait for monitor refresh

    // ------------------------------------------------------------------
    // 3. Load the assets.
    //    Ownership: these resources are about to be HANDED OVER to our
    //    classes. Their destructors will free them - we don't free them
    //    here.
    // ------------------------------------------------------------------
    SDL_Texture* spriteTexture = IMG_LoadTexture(renderer, "sprite_sheet.png");
    TTF_Font*    font          = TTF_OpenFont("RobotoMono-Light.ttf", 28);

    if (!spriteTexture || !font) {
        SDL_Log("Asset load failed: %s", SDL_GetError());
        if (spriteTexture) SDL_DestroyTexture(spriteTexture);
        if (font)          TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    // Smooth scaling looks better than nearest-neighbour when scaling up.
    SDL_SetTextureScaleMode(spriteTexture, SDL_SCALEMODE_LINEAR);

    // ------------------------------------------------------------------
    // 4. Construct our objects, run the game loop, then let them go.
    //
    //    Why the extra { } block? It controls SCOPE - and therefore
    //    LIFETIME. `player` and `hud` are local objects: they are
    //    destroyed automatically when their enclosing scope ends. We
    //    want their destructors (which call SDL_DestroyTexture and
    //    TTF_CloseFont) to run BEFORE we tear down the renderer and
    //    SDL itself. Wrapping them in a block guarantees that order.
    // ------------------------------------------------------------------
    {
        Player player(spriteTexture,
                      /*frameCount   =*/ 6,
                      /*frameDuration=*/ 1.0f / 8.0f,   // 12 fps run cycle
                      static_cast<float>(screenW),
                      static_cast<float>(screenH));

        HUD hud(font, renderer);

        // ---- The main loop ------------------------------------------
        bool   running   = true;
        Uint64 lastTicks = SDL_GetTicksNS();

        while (running) {

            // -- handle input -----------------------------------------
            SDL_Event ev;
            while (SDL_PollEvent(&ev)) {
                if (ev.type == SDL_EVENT_QUIT) {
                    running = false;
                }
                if (ev.type == SDL_EVENT_KEY_DOWN &&
                    ev.key.scancode == SDL_SCANCODE_ESCAPE) {
                    running = false;
                }
            }

            // -- compute dt: seconds since the previous frame ---------
            const Uint64 now = SDL_GetTicksNS();
            const float  dt  = static_cast<float>(now - lastTicks) / 1.0e9f;
            lastTicks = now;

            // -- update everything ------------------------------------
            // Look how short this is. THIS is what well-designed
            // classes buy you - a top level you can read at a glance.
            player.update(dt);
            hud.update(dt);

            // -- draw everything --------------------------------------
            SDL_SetRenderDrawColor(renderer, 30, 35, 45, 255);
            SDL_RenderClear(renderer);
            player.render(renderer);
            hud.render(renderer);
            SDL_RenderPresent(renderer);
        }
    }
    // <-- player and hud are destroyed HERE, releasing texture and font.

    // ------------------------------------------------------------------
    // 5. Tear down SDL itself.
    // ------------------------------------------------------------------
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
