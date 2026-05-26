#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "Common.h"

class GlyphCache;
class Map;

// ============================================================
//  FlashEffects  -  short-lived per-tile highlights.
// ============================================================
//
//  Used to flash a tile briefly red when something on it takes
//  damage. Each flash has a position, a color, and an expiry
//  timestamp. The Game uses `any(now)` to decide whether to
//  use SDL_WaitEventTimeout (animations active) or
//  SDL_WaitEvent (truly idle).
// ============================================================

struct Flash
{
    Point     p;
    SDL_Color color;
    Uint64    expireMs;
};

class FlashEffects
{
public:
    void add(Point p, SDL_Color color, int durationMs, Uint64 nowMs);
    void prune(Uint64 nowMs);

    bool any(Uint64 nowMs) const;
    bool empty()           const { return flashes_.empty(); }

    void clear() { flashes_.clear(); }

    // Draws a solid filled rectangle in the flash color over the affected
    // cell. Fade-out happens automatically via the expire timestamp.
    void render(SDL_Renderer* renderer, Uint64 nowMs) const;

private:
    std::vector<Flash> flashes_;
};
