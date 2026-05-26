#include "HUD.h"
#include "GlyphCache.h"
#include "Player.h"
#include "Common.h"

#include <cstdio>

void HUD::addMessage(const std::string& msg)
{
    messages_.push_back(msg);
    // Keep only the most recent few; dropping old ones from the front.
    while ((int)messages_.size() > MAX_MESSAGES)
        messages_.erase(messages_.begin());
}

void HUD::clear()
{
    messages_.clear();
}

void HUD::render(SDL_Renderer* renderer,
                 const GlyphCache& glyphs,
                 const Player& player,
                 int depth,
                 bool gameOver) const
{
    const int row0 = MAP_H;        // first HUD row

    // Status line.
    char buf[128];
    SDL_snprintf(buf, sizeof(buf),
                 "HP: %d/%d   Gold: %d   Potions: %d   Depth: %d   "
                 "%c heal  %c descend",
                 player.hp(), player.maxHp(),
                 player.gold(),
                 player.potions(),
                 depth,
                 'h', '.');
    glyphs.drawText(renderer, buf, 1, row0, Palette::HUD_TEXT);

    // Game over flash.
    if (gameOver)
    {
        glyphs.drawText(renderer,
            "YOU DIED. Press R to start a new run.  Esc to quit.",
            1, row0 + 1, Palette::HUD_BAD);
        return;
    }

    // Message log.
    for (size_t i = 0; i < messages_.size(); ++i)
    {
        // Older messages dim a little.
        SDL_Color c = Palette::HUD_TEXT;
        if (i + 1 < messages_.size()) c = Palette::HUD_DIM;
        glyphs.drawText(renderer, messages_[i].c_str(),
                        1, row0 + 1 + (int)i, c);
    }
}
