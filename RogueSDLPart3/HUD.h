#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <vector>

class GlyphCache;
class Player;

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

private:
    std::vector<std::string> messages_;
};
