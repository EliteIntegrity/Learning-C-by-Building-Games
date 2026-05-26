#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

class GlyphCache;
class Player;

// ============================================================
//  HUD  -  text-based status display + message log.
// ============================================================
//
//  Lives in the bottom HUD_ROWS rows of the window:
//
//    Row MAP_H + 0     "HP: 12/20  Gold: 47  Potions: 2  Depth: 3"
//    Row MAP_H + 1..4  the most recent 4 messages
//
//  Messages are pushed at the bottom of the log; older ones
//  dim as they scroll up.
// ============================================================

class HUD
{
public:
    HUD() = default;

    void addMessage(const std::string& msg);
    void clear();

    void render(SDL_Renderer* renderer,
                const GlyphCache& glyphs,
                const Player& player,
                int depth,
                bool gameOver) const;

    const std::vector<std::string>& messages() const { return messages_; }

private:
    std::vector<std::string> messages_;
};
