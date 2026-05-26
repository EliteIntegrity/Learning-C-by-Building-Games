#include "Flash.h"

void FlashEffects::add(Point p, SDL_Color color, int durationMs, Uint64 nowMs)
{
    flashes_.push_back({ p, color, nowMs + (Uint64)durationMs });
}

void FlashEffects::prune(Uint64 nowMs)
{
    for (size_t i = flashes_.size(); i > 0; --i)
        if (flashes_[i - 1].expireMs <= nowMs)
            flashes_.erase(flashes_.begin() + (i - 1));
}

bool FlashEffects::any(Uint64 nowMs) const
{
    for (const Flash& f : flashes_)
        if (f.expireMs > nowMs) return true;
    return false;
}

void FlashEffects::render(SDL_Renderer* renderer, Uint64 nowMs) const
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for (const Flash& f : flashes_)
    {
        if (f.expireMs <= nowMs) continue;
        // Fade out: alpha falls from current alpha down to 0 over the
        // remaining lifetime.
        Sint64 remaining = (Sint64)f.expireMs - (Sint64)nowMs;
        if (remaining < 0) continue;
        // Assume FLASH_MS as the original lifetime; the ratio gives alpha scale.
        float t = (float)remaining / (float)FLASH_MS;
        if (t > 1.0f) t = 1.0f;
        Uint8 alpha = (Uint8)(f.color.a * t);
        SDL_SetRenderDrawColor(renderer, f.color.r, f.color.g, f.color.b, alpha);

        SDL_FRect r {
            (float)(f.p.x * CELL_PX),
            (float)(f.p.y * CELL_PX),
            (float)CELL_PX,
            (float)CELL_PX
        };
        SDL_RenderFillRect(renderer, &r);
    }
}
