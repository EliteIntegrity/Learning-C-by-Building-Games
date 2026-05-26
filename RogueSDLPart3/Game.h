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
#include "Sound.h"
#include "Flash.h"
#include "GameState.h"

// ============================================================
//  Game  -  world container + state stack manager.
// ============================================================
//
//  Owns every long-lived bit of state: the map, the player,
//  the entity vectors, the HUD, the glyph cache, the sounds,
//  the flash effects, and the depth counter.
//
//  Also owns a std::vector<std::unique_ptr<GameState>> stack.
//  All input goes to the TOP state; rendering walks the stack
//  bottom-to-top.
// ============================================================

class Game
{
public:
    explicit Game(SDL_Renderer* renderer);
    ~Game() = default;

    Game(const Game&)            = delete;
    Game& operator=(const Game&) = delete;

    void newGame();
    void run();

    // ---- World access (states use these) ----
    Map&                 map()       { return map_; }
    const Map&           map() const { return map_; }
    Player&              player()    { return *player_; }
    HUD&                 hud()       { return hud_; }
    Sounds&              sounds()    { return sounds_; }
    GlyphCache&          glyphs()    { return glyphs_; }
    FlashEffects&        flashes()   { return flashes_; }
    SDL_Renderer*        renderer()  { return renderer_; }

    std::vector<std::unique_ptr<Enemy>>& enemies() { return enemies_; }
    std::vector<std::unique_ptr<Item>>&  items()   { return items_; }

    int  depth() const { return depth_; }
    bool gameOver() const { return gameOver_; }
    void setGameOver(bool v) { gameOver_ = v; }
    bool quit() const  { return quit_; }
    void requestQuit() { quit_ = true; }

    Uint64 nowMs() const { return SDL_GetTicks(); }

    // ---- Stack management ----
    void pushState(std::unique_ptr<GameState> s);
    void popState();
    GameState* top();

    // ---- World actions states call ----
    void recomputeFov();
    void enemyTurns();              // resolves AI + cleans dead enemies
    void descend();                 // depth + 1, regenerate
    Enemy* enemyAt(Point p);
    Item*  itemAt(Point p);

    void playerAttack(Enemy& target);
    void enemyAttack(Enemy& attacker);

    void doMagicMapping();
    void castFireballAt(Point center);

    void save();
    void load();

private:
    void render();

    SDL_Renderer* renderer_;
    GlyphCache    glyphs_;
    Map           map_;
    MapGenerator  gen_;
    HUD           hud_;
    Sounds        sounds_;
    FlashEffects  flashes_;

    std::unique_ptr<Player>             player_;
    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::unique_ptr<Item>>  items_;

    std::vector<std::unique_ptr<GameState>> stack_;

    int  depth_    = 1;
    bool quit_     = false;
    bool gameOver_ = false;
    bool dirty_    = true;
};
