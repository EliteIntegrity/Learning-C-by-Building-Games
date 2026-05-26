#pragma once
#include <memory>
#include <vector>
#include "Common.h"
#include "Map.h"
#include "MapGen.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "GlyphCache.h"
#include "HUD.h"

class Game
{
public:
    explicit Game(SDL_Renderer* renderer);
    ~Game() = default;

    Game(const Game&)            = delete;
    Game& operator=(const Game&) = delete;

    void newGame();                   // depth 1, reset player
    void descend();                   // depth + 1, keep player stats
    void run();

private:
    bool handleEvent(const SDL_Event& ev);   // returns true if turn taken
    void enemyTurns();
    void render();
    void recomputeFov();

    // Combat
    void playerAttack(Enemy& target);
    void enemyAttack(Enemy& attacker);

    // World lookup
    Enemy* enemyAt(Point p);
    Item*  itemAt(Point p);

    // Save/load
    void save();
    void load();

    SDL_Renderer* renderer_;
    GlyphCache    glyphs_;
    Map           map_;
    MapGenerator  gen_;
    HUD           hud_;

    std::unique_ptr<Player>            player_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::unique_ptr<Item>>  items_;

    int   depth_   = 1;
    bool  quit_    = false;
    bool  dirty_   = true;
    bool  gameOver_ = false;
};
