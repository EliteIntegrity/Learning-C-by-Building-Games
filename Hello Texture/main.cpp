/*
 * ============================================================
 *  SDL3_image — Why is it a separate library?
 * ============================================================
 *
 *  SDL3 itself only knows how to load BMP images.  That is
 *  intentional: BMP support is tiny, has no extra dependencies,
 *  and covers the bare minimum needed to get pixels on screen.
 *
 *  Real projects need PNG, JPEG, WebP, TIFF, and more.  Those
 *  formats require their own third-party decode libraries
 *  (libpng, libjpeg, etc.).  Bundling all of them into SDL3
 *  would bloat every SDL3 program with code most games never
 *  use.  Instead, the SDL team ships SDL3_image as an optional
 *  add-on that you link only when you actually need it.
 *
 *  That is why you have TWO DLLs next to your .exe:
 *    SDL3.dll        — the core library  (window, renderer, input …)
 *    SDL3_image.dll  — the image loader add-on (PNG, JPEG, …)
 *
 *  You link SDL3.lib in your project settings as usual, and you
 *  ALSO link SDL3_image.lib.  Both DLLs must travel with your
 *  executable when you ship your game.
 * ============================================================
 */

#include <SDL3/SDL.h>
#include <SDL_image.h>   // extra header from SDL3_image

#include <cmath>               // for sinf / cosf (used by SDL internally, good habit)

// ---------------------------------------------------------------------------
//  Window and renderer dimensions
// ---------------------------------------------------------------------------
static const int WINDOW_W = 800;
static const int WINDOW_H = 600;

int main(int /*argc*/, char* /*argv*/[])
{
    // -----------------------------------------------------------------------
    //  1. Initialise SDL3
    //     SDL_INIT_VIDEO starts the video subsystem (window + renderer).
    // -----------------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // -----------------------------------------------------------------------
    //  2. Create a window
    //     The last argument is a flags bitmask; 0 means no special flags.
    // -----------------------------------------------------------------------
    SDL_Window* window = SDL_CreateWindow(
        "Hello Texture – bouncing ship",   // title bar text
        WINDOW_W, WINDOW_H,                // size in pixels
        0                                  // flags (0 = plain window)
    );
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // -----------------------------------------------------------------------
    //  3. Create a hardware-accelerated renderer attached to the window.
    //     The renderer is what draws everything — textures, shapes, etc.
    // -----------------------------------------------------------------------
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // -----------------------------------------------------------------------
    //  4. Load the PNG with SDL3_image.
    //
    //     IMG_LoadTexture does two things in one call:
    //       a) decodes the PNG file into raw pixel data (SDL3_image work)
    //       b) uploads those pixels to the GPU as a texture (SDL3 work)
    //
    //     If you only had SDL3 you would have to save the image as a BMP
    //     and use SDL_LoadBMP + SDL_CreateTextureFromSurface instead.
    // -----------------------------------------------------------------------
    SDL_Texture* shipTexture = IMG_LoadTexture(renderer, "ship.png");
    if (!shipTexture)
    {
        SDL_Log("IMG_LoadTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Get the natural size of the texture so we can centre-pivot rotations
    float texW = 0.0f, texH = 0.0f;
    SDL_GetTextureSize(shipTexture, &texW, &texH);

    // -----------------------------------------------------------------------
    //  5. Set up the ship's initial position and velocity.
    //
    //     pos   — top-left corner of the destination rectangle
    //     vel   — pixels per second in X and Y directions
    //     angle — current rotation angle in degrees
    //     spin  — degrees per second the ship rotates
    // -----------------------------------------------------------------------
    float posX = (WINDOW_W - texW) / 2.0f;   // start at screen centre
    float posY = (WINDOW_H - texH) / 2.0f;

    float velX = 200.0f;   // pixels per second
    float velY = 150.0f;

    float angle = 0.0f;
    float spin  = 90.0f;   // degrees per second

    // -----------------------------------------------------------------------
    //  6. Track time so movement is frame-rate independent.
    //     SDL_GetTicks returns milliseconds since SDL was initialised.
    // -----------------------------------------------------------------------
    Uint64 lastTime = SDL_GetTicks();

    // -----------------------------------------------------------------------
    //  7. Main game loop
    // -----------------------------------------------------------------------
    bool running = true;
    while (running)
    {
        // --- 7a. Calculate delta time (seconds since last frame) ------------
        Uint64 now       = SDL_GetTicks();
        float  deltaTime = (now - lastTime) / 1000.0f;   // ms → seconds
        lastTime = now;

        // --- 7b. Handle events (keyboard, window close, etc.) ---------------
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            // Press Escape to quit
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)
                running = false;
        }

        // --- 7c. Update position and angle ----------------------------------

        // Move the ship
        posX += velX * deltaTime;
        posY += velY * deltaTime;

        // Spin the ship
        angle += spin * deltaTime;

        // Bounce off the left and right edges
        if (posX < 0.0f)
        {
            posX = 0.0f;
            velX = -velX;
        }
        else if (posX + texW > WINDOW_W)
        {
            posX = WINDOW_W - texW;
            velX = -velX;
        }

        // Bounce off the top and bottom edges
        if (posY < 0.0f)
        {
            posY = 0.0f;
            velY = -velY;
        }
        else if (posY + texH > WINDOW_H)
        {
            posY = WINDOW_H - texH;
            velY = -velY;
        }

        // --- 7d. Draw -------------------------------------------------------

        // Clear the screen to a dark navy colour (R,G,B,A each 0-255)
        SDL_SetRenderDrawColor(renderer, 15, 20, 40, 255);
        SDL_RenderClear(renderer);

        // Build the destination rectangle (where on screen to draw the ship)
        SDL_FRect dest;
        dest.x = posX;
        dest.y = posY;
        dest.w = texW;
        dest.h = texH;

        // SDL_RenderTextureRotated draws the texture with a rotation.
        //   nullptr  — use the whole texture (no source crop)
        //   &dest    — where to draw it on screen
        //   angle    — rotation in degrees (clockwise)
        //   nullptr  — rotate around the centre of dest (default pivot)
        //   SDL_FLIP_NONE — no horizontal/vertical flip
        SDL_RenderTextureRotated(renderer, shipTexture,
                                 nullptr, &dest,
                                 angle, nullptr,
                                 SDL_FLIP_NONE);

        // Show the finished frame (swaps the back buffer to the screen)
        SDL_RenderPresent(renderer);
    }

    // -----------------------------------------------------------------------
    //  8. Clean up — always destroy resources in reverse order of creation
    // -----------------------------------------------------------------------
    SDL_DestroyTexture(shipTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
