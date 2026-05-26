#pragma once
#include <memory>
#include "Common.h"
#include "Map.h"
#include "MapGen.h"
#include "Player.h"
#include "GlyphCache.h"

// ============================================================
//  Game  -  the top-level orchestrator.
// ============================================================
//
//  Owns the map, the player, the glyph cache, and the
//  procgen. Runs the main loop. In Chapter 28 it does just
//  these things; Chapter 29 grows it with enemies, items,
//  combat, multiple levels, and a message log.
// ============================================================

class Game
{
public:
    explicit Game(SDL_Renderer* renderer);
    ~Game() = default;

    Game(const Game&)            = delete;
    Game& operator=(const Game&) = delete;

    // Build the starting world.
    void newDungeon();

    // Run until the player quits.
    void run();

private:
    bool handleEvent(const SDL_Event& ev);   // returns true if a turn was taken
    void render();
    void recomputeFov();

    void save();
    void load();

    SDL_Renderer*           renderer_;
    GlyphCache              glyphs_;
    Map                     map_;
    MapGenerator            gen_;
    std::unique_ptr<Player> player_;

    bool quit_  = false;
    bool dirty_ = true;   // do we need to redraw?
};
