#include "HUD.h"
#include "GlyphCache.h"
#include "Player.h"
#include "Common.h"
#include "Item.h"

#include <cstdio>

void HUD::addMessage(const std::string& msg)
{
    messages_.push_back(msg);
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
    const int row0 = MAP_H;

    char buf[160];
    SDL_snprintf(buf, sizeof(buf),
        "HP: %d/%d   Gold: %d   Dmg: %d   Depth: %d   Wielded: %s",
        player.hp(), player.maxHp(),
        player.gold(),
        player.effectiveDamage(),
        depth,
        itemDisplayName(player.wielded()));
    glyphs.drawText(renderer, buf, 1, row0, Palette::HUD_TEXT);

    // Always-visible help on the second row.
    glyphs.drawText(renderer,
        "i: inventory   .: descend   F5/F9: save/load   Esc: quit",
        1, row0 + 1, Palette::HUD_DIM);

    if (gameOver)
    {
        glyphs.drawText(renderer,
            "YOU DIED. Press R to start a new run.",
            1, row0 + 2, Palette::HUD_BAD);
        return;
    }

    // Message log fills the remaining rows.
    int startRow = row0 + 2;
    int rowsLeft = HUD_ROWS - 2;
    size_t startIdx = (messages_.size() > (size_t)rowsLeft)
        ? messages_.size() - rowsLeft
        : 0;
    for (size_t i = startIdx; i < messages_.size(); ++i)
    {
        SDL_Color c = (i + 1 < messages_.size()) ? Palette::HUD_DIM
                                                  : Palette::HUD_TEXT;
        glyphs.drawText(renderer, messages_[i].c_str(),
                        1, startRow + (int)(i - startIdx), c);
    }
}
