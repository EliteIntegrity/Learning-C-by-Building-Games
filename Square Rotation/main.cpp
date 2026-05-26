/* ============================================================================
 *  Square Rotation — SDL3 Tutorial
 * ============================================================================
 *
 *  OVERVIEW
 *  --------
 *  Draws a square centred on screen and rotates it smoothly over time.
 *  The rotation is frame-rate independent: we measure how many seconds
 *  elapsed since the last frame (delta time) and advance the angle by
 *  ROT_SPEED * dt, so the square spins at the same visual speed on a
 *  30 Hz monitor as on a 144 Hz one.
 *
 * ============================================================================
 *
 *  THE ROTATION MATHS
 *  ------------------
 *  A square's four corners are evenly spaced 90° apart on a circle whose
 *  radius is the distance from the centre to any corner (half the diagonal).
 *
 *                    corner
 *                   /
 *          radius  /        half-diagonal = HALF_SIZE * sqrt(2)
 *                 /
 *         centre *--------  HALF_SIZE
 *
 *  Starting the first corner at 45° (instead of 0°) makes the square sit
 *  flat; without that offset the square would appear as a diamond.
 *
 *    corner[i].x = cx + cos(angle + 45° + i * 90°) * radius
 *    corner[i].y = cy + sin(angle + 45° + i * 90°) * radius
 *
 *  Incrementing `angle` by ROT_SPEED * dt each frame spins the square.
 *
 * ============================================================================
 *
 *  WHY SDL MATH INSTEAD OF <cmath>?
 *  ---------------------------------
 *  SDL ships portable math wrappers in SDL_stdinc.h (included via SDL.h):
 *
 *    +-----------------------+----------------------------------------------+
 *    |  SDL function         |  Role in this program                        |
 *    +-----------------------+----------------------------------------------+
 *    |  SDL_sqrtf( x )       |  Compute the corner-circle radius once,      |
 *    |                       |  before the main loop.                       |
 *    +-----------------------+----------------------------------------------+
 *    |  SDL_cosf( a )        |  X component of each rotated corner.         |
 *    |  SDL_sinf( a )        |  Y component of each rotated corner.         |
 *    +-----------------------+----------------------------------------------+
 *    |  SDL_PI_F             |  Portable float pi constant (3.14159…).      |
 *    +-----------------------+----------------------------------------------+
 *
 *  Benefits over std::sin / std::cos / std::sqrt:
 *
 *    1. No extra header or linker flag.  No #include <cmath> is needed, and
 *       on Linux you do not have to pass -lm; SDL already handles whatever
 *       the platform requires.
 *
 *    2. Consistent, guaranteed availability.  SDL targets platforms
 *       (consoles, embedded systems) where the C runtime math library may
 *       be missing, restricted, or have known precision quirks.  SDL's
 *       wrappers paper over those differences, so the same source compiles
 *       and behaves correctly everywhere SDL runs.
 *
 *  For a desktop-only project the difference is academic, but forming the
 *  habit keeps code portable from the start.
 *
 * ============================================================================
 *
 *  RENDERING
 *  ---------
 *  corners[5] with corners[4] = corners[0] closes the square outline.
 *  SDL_RenderLines draws a connected strip of line segments, so the 5th
 *  point brings the final line back to the starting corner.
 *
 * ============================================================================
 */

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>   // lets SDL redirect WinMain -> main on Windows

// ---------------------------------------------------------------------------
// Configuration constants
// ---------------------------------------------------------------------------

static const int   WINDOW_W  = 800;    // window width  in pixels
static const int   WINDOW_H  = 600;    // window height in pixels
static const float HALF_SIZE = 80.0f;  // half the side length of the square
static const float ROT_SPEED = 1.5f;   // rotation speed in radians per second

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // SDL_Init returns false on failure (SDL3 uses bool, not int)
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window*   window   = SDL_CreateWindow("Square Rotation", WINDOW_W, WINDOW_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL); // NULL = default driver

    if (!window || !renderer) {
        SDL_Log("Could not create window/renderer: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // The square's four corners sit on a circle whose radius equals the
    // distance from the centre to a corner (half the diagonal).
    //   half-diagonal = HALF_SIZE * sqrt(2)
    // We compute it once here because HALF_SIZE never changes.
    const float CORNER_RADIUS = HALF_SIZE * SDL_sqrtf(2.0f);

    float  angle    = 0.0f;           // current rotation in radians
    Uint64 lastTime = SDL_GetTicks(); // SDL3: GetTicks returns milliseconds as Uint64

    bool running = true;
    while (running)
    {
        // --- Event handling -------------------------------------------------
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // --- Delta time -----------------------------------------------------
        Uint64 now = SDL_GetTicks();
        float  dt  = (float)(now - lastTime) / 1000.0f; // convert ms -> seconds
        lastTime   = now;

        angle += ROT_SPEED * dt; // advance rotation

        // --- Compute the four rotated corners -------------------------------
        // A square's corners are evenly spaced at 90-degree intervals on a
        // circle.  We start at 45 degrees (SDL_PI_F / 4) so the square sits
        // flat rather than as a diamond, then add the current angle on top.
        //
        //   corner.x = cx + cos(angle + 45° + i*90°) * CORNER_RADIUS
        //   corner.y = cy + sin(angle + 45° + i*90°) * CORNER_RADIUS

        const float cx = WINDOW_W / 2.0f;  // screen centre x
        const float cy = WINDOW_H / 2.0f;  // screen centre y

        SDL_FPoint corners[5]; // index 4 repeats index 0 to close the outline
        for (int i = 0; i < 4; i++) {
            float a      = angle + (SDL_PI_F / 4.0f) + i * (SDL_PI_F / 2.0f);
            corners[i].x = cx + SDL_cosf(a) * CORNER_RADIUS;
            corners[i].y = cy + SDL_sinf(a) * CORNER_RADIUS;
        }
        corners[4] = corners[0]; // close the loop

        // --- Render ---------------------------------------------------------
        SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);   // dark blue background
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 220, 180, 60, 255); // golden square outline
        SDL_RenderLines(renderer, corners, 5);

        SDL_RenderPresent(renderer); // swap front/back buffers
    }

    // Clean up in reverse order of creation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
