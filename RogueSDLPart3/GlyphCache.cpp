#include "GlyphCache.h"
#include "Common.h"

GlyphCache::GlyphCache(SDL_Renderer* renderer, const char* fontPath, int ptSize)
{
    TTF_Font* font = TTF_OpenFont(fontPath, (float)ptSize);
    if (!font)
    {
        SDL_Log("TTF_OpenFont failed for %s: %s", fontPath, SDL_GetError());
        return;
    }

    const SDL_Color white = { 255, 255, 255, 255 };

    for (char c = 32; c < 127; ++c)
    {
        char buf[2] = { c, 0 };
        SDL_Surface* surf = TTF_RenderText_Blended(font, buf, 1, white);
        if (!surf) continue;

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        if (tex)
        {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            textures_[c] = tex;
            if (glyphW_ == 0) { glyphW_ = surf->w; glyphH_ = surf->h; }
        }
        SDL_DestroySurface(surf);
    }

    TTF_CloseFont(font);
}

GlyphCache::~GlyphCache()
{
    for (auto& [ch, tex] : textures_)
        SDL_DestroyTexture(tex);
    textures_.clear();
}

void GlyphCache::draw(SDL_Renderer* renderer,
                      char ch, int cellX, int cellY,
                      SDL_Color color) const
{
    auto it = textures_.find(ch);
    if (it == textures_.end()) return;

    SDL_Texture* tex = it->second;
    SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(tex, color.a);

    float dstX = (float)(cellX * CELL_PX + (CELL_PX - glyphW_) / 2);
    float dstY = (float)(cellY * CELL_PX + (CELL_PX - glyphH_) / 2);
    SDL_FRect dst { dstX, dstY, (float)glyphW_, (float)glyphH_ };
    SDL_RenderTexture(renderer, tex, nullptr, &dst);
}

void GlyphCache::drawText(SDL_Renderer* renderer,
                          const char* text, int cellX, int cellY,
                          SDL_Color color) const
{
    for (int i = 0; text[i] != '\0'; ++i)
        draw(renderer, text[i], cellX + i, cellY, color);
}
