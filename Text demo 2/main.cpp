/*
 * SDL3 Hello Text Demo
 * ====================
 *
 * A short tutorial demonstrating TrueType text rendering with SDL3 and SDL3_ttf.
 *
 * What this demo does:
 *   - Opens a window and a hardware-accelerated renderer.
 *   - Loads RobotoMono-Light.ttf with TTF_OpenFont.
 *   - Rasterises each character of "Hello, World!" into its own GPU texture
 *     so that every glyph can be moved and coloured independently.
 *   - Animates the characters as a travelling rainbow wave:
 *       * Each character bobs vertically on a sine wave. A phase offset per
 *         character makes the ripple travel from left to right.
 *       * Each character is tinted a different hue using SDL_SetTextureColorMod,
 *         and the whole palette scrolls over time to give a flowing rainbow.
 *
 * Press Escape or close the window to exit.
 */

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <string>
#include <vector>

static constexpr int   SCREEN_W        = 800;
static constexpr int   SCREEN_H        = 400;
static constexpr float FONT_PT         = 64.0f;

// Wave animation parameters
static constexpr float WAVE_AMPLITUDE  = 38.0f;   // vertical travel in pixels
static constexpr float WAVE_SPEED      = 2.8f;    // radians per second
static constexpr float WAVE_PHASE_STEP = 0.4f;    // phase gap between adjacent chars

static constexpr float HUE_SCROLL_RATE = 50.0f;   // degrees of hue per second

// Convert a hue angle (degrees) to an RGB colour at full saturation and brightness.
static SDL_Color HueToRGB(float hue)
{
    hue = fmodf(hue, 360.0f);
    if (hue < 0.0f) hue += 360.0f;
    int   sector = (int)(hue / 60.0f) % 6;
    float frac   = hue / 60.0f - floorf(hue / 60.0f);
    Uint8 q = (Uint8)((1.0f - frac) * 255.0f);
    Uint8 t = (Uint8)(frac         * 255.0f);
    switch (sector) {
        case 0: return { 255,   t,   0, 255 };
        case 1: return {   q, 255,   0, 255 };
        case 2: return {   0, 255,   t, 255 };
        case 3: return {   0,   q, 255, 255 };
        case 4: return {   t,   0, 255, 255 };
        default:return { 255,   0,   q, 255 };
    }
}

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // TTF_Init() starts the SDL3_ttf font engine.
    // Must be called before any other TTF_ function.
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window*   window   = SDL_CreateWindow("SDL3 Hello Text", SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    // TTF_OpenFont loads a .ttf file from disk and prepares it at the requested
    // point size. A higher point size produces sharper glyphs at large sizes
    // but uses more texture memory per glyph.
    TTF_Font* font = TTF_OpenFont("RobotoMono-Light.ttf", FONT_PT);
    if (!font) {
        SDL_Log("TTF_OpenFont failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // --- Measure the monospace character cell ---
    // TTF_GetStringSize returns the pixel dimensions that a rendered string
    // would occupy. For RobotoMono (a monospace font) every character has
    // the same cell width, so one reference character is enough.
    int refW = 0, refH = 0;
    TTF_GetStringSize(font, "M", 0, &refW, &refH);
    float cellW = (float)refW;
    float cellH = (float)refH;

    // --- Build per-character textures ---
    // We rasterise each character individually so we can position and tint
    // them separately every frame. Spaces are stored as nullptr placeholders.
    const std::string message = "Hello, World!";
    std::vector<SDL_Texture*> glyphs;

    for (char c : message)
    {
        if (c == ' ') {
            glyphs.push_back(nullptr);
            continue;
        }

        char buf[2] = { c, '\0' };

        // TTF_RenderText_Blended rasterises the string with smooth anti-aliasing
        // onto an ARGB surface with a transparent background.
        // We render in white (255, 255, 255) so that SDL_SetTextureColorMod
        // can tint the glyph to any colour later — white acts as a neutral base.
        SDL_Surface* surf = TTF_RenderText_Blended(font, buf, 0, { 255, 255, 255, 255 });

        // SDL_CreateTextureFromSurface uploads the pixel data to the GPU.
        // After this we hold a lightweight handle to GPU memory rather than
        // a full CPU-side pixel buffer.
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

        // SDL3 uses SDL_DestroySurface (SDL2 used SDL_FreeSurface).
        SDL_DestroySurface(surf);

        // Enable alpha blending so the transparent glyph background
        // lets the dark scene show through correctly.
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

        glyphs.push_back(tex);
    }

    // Centre the text block horizontally on screen.
    float totalWidth = cellW * (float)message.size();
    float startX     = (SCREEN_W - totalWidth) * 0.5f;
    float baseY      = SCREEN_H * 0.5f;

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)                   running = false;
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.key == SDLK_ESCAPE)                   running = false;
        }

        // SDL_GetTicks() returns milliseconds since SDL_Init.
        // Multiplying by 0.001 gives a smooth float time in seconds.
        float t = (float)SDL_GetTicks() * 0.001f;

        // Dark navy background so the bright rainbow text pops.
        SDL_SetRenderDrawColor(renderer, 12, 12, 28, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < (int)glyphs.size(); ++i)
        {
            if (glyphs[i] == nullptr) continue;   // skip spaces

            // --- Travelling wave ---
            // Adding (i * WAVE_PHASE_STEP) to the angle shifts each character's
            // sine phase so the crest moves from left to right across the string.
            float yOff = WAVE_AMPLITUDE * sinf(t * WAVE_SPEED + i * WAVE_PHASE_STEP);

            float x = startX + i * cellW;
            float y = baseY  + yOff;

            // --- Rainbow colour ---
            // The hue base advances with time, and we add (i * spread) to
            // distribute the full 360° of hue evenly across all characters.
            // The result is a rainbow that scrolls slowly to the right.
            float spread = 360.0f / (float)message.size();
            float hue    = t * HUE_SCROLL_RATE + i * spread;
            SDL_Color col = HueToRGB(hue);

            // SDL_SetTextureColorMod multiplies the texture's RGB channels
            // by (r, g, b) / 255. Because our glyphs are white, the result
            // is exactly the chosen colour — a neat way to recolour a texture
            // without re-uploading it to the GPU each frame.
            SDL_SetTextureColorMod(glyphs[i], col.r, col.g, col.b);

            // SDL3 uses SDL_FRect (floating-point rectangle) for all render
            // destinations, which allows smooth sub-pixel positioning.
            SDL_FRect dst = {
                x,
                y - cellH * 0.5f,   // vertically centre on the wave baseline
                cellW,
                cellH
            };
            SDL_RenderTexture(renderer, glyphs[i], nullptr, &dst);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);   // ~60 fps
    }

    // --- Cleanup ---
    // Always destroy GPU textures before the renderer that owns them.
    for (SDL_Texture* tex : glyphs)
        if (tex) SDL_DestroyTexture(tex);

    // TTF_CloseFont frees the font's internal glyph cache and file data.
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // TTF_Quit() shuts down the SDL3_ttf subsystem — pair with every TTF_Init().
    TTF_Quit();
    SDL_Quit();

    return 0;
}
