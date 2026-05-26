#include "Game.h"
#include "FOV.h"
#include "Pathfind.h"
#include "SaveLoad.h"
#include "PlayingState.h"

#include <cstdio>
#include <algorithm>

static const char* SAVE_PATH = "rogue_sdl_save.txt";
static const char* FONT_PATH = "RobotoMono-Light.ttf";

Game::Game(SDL_Renderer* renderer)
    : renderer_(renderer)
    , glyphs_(renderer, FONT_PATH, 18)
    , map_(MAP_W, MAP_H)
    , gen_()
{
    sounds_.loadAll();
}

void Game::pushState(std::unique_ptr<GameState> s)
{
    GameState* raw = s.get();
    stack_.push_back(std::move(s));
    raw->onEnter(*this);
    dirty_ = true;
}

void Game::popState()
{
    if (stack_.empty()) return;
    stack_.back()->onExit(*this);
    stack_.pop_back();
    dirty_ = true;
}

GameState* Game::top()
{
    return stack_.empty() ? nullptr : stack_.back().get();
}

void Game::newGame()
{
    depth_    = 1;
    gameOver_ = false;
    hud_.clear();
    flashes_.clear();
    stack_.clear();

    Point start = gen_.generate(map_, enemies_, items_, depth_);
    player_ = std::make_unique<Player>(start);
    player_->resetForNewGame(start);
    recomputeFov();

    hud_.addMessage("Welcome to Rogue SDL.  Press i for inventory.");
    pushState(std::make_unique<PlayingState>());
    dirty_ = true;
}

void Game::descend()
{
    ++depth_;
    Point start = gen_.generate(map_, enemies_, items_, depth_);
    player_->setPosition(start);
    recomputeFov();
    sounds_.descend.play();

    char buf[64];
    SDL_snprintf(buf, sizeof(buf), "You descend to depth %d.", depth_);
    hud_.addMessage(buf);
    dirty_ = true;
}

void Game::recomputeFov()
{
    FOV::compute(map_, player_->position(), FOV_RADIUS);
}

Enemy* Game::enemyAt(Point p)
{
    for (auto& e : enemies_)
        if (e->alive() && e->position() == p) return e.get();
    return nullptr;
}

Item* Game::itemAt(Point p)
{
    for (auto& it : items_)
        if (it->position() == p) return it.get();
    return nullptr;
}

void Game::playerAttack(Enemy& target)
{
    int dmg = player_->effectiveDamage();
    target.takeDamage(dmg);
    sounds_.hit.play();
    flashes_.add(target.position(), Palette::FLASH_RED, FLASH_MS, nowMs());

    const char* name = "enemy";
    switch (target.kind())
    {
        case MonsterKind::Rat:    name = "rat";    break;
        case MonsterKind::Goblin: name = "goblin"; break;
        case MonsterKind::Orc:    name = "orc";    break;
    }
    char buf[96];
    if (target.alive())
    {
        SDL_snprintf(buf, sizeof(buf), "You hit the %s for %d.", name, dmg);
    }
    else
    {
        SDL_snprintf(buf, sizeof(buf), "You kill the %s!", name);
        sounds_.kill.play();
    }
    hud_.addMessage(buf);
}

void Game::enemyAttack(Enemy& attacker)
{
    int dmg = attacker.damage();
    player_->takeDamage(dmg);
    sounds_.playerHurt.play();
    flashes_.add(player_->position(), Palette::FLASH_RED, FLASH_MS, nowMs());

    const char* name = "enemy";
    switch (attacker.kind())
    {
        case MonsterKind::Rat:    name = "rat";    break;
        case MonsterKind::Goblin: name = "goblin"; break;
        case MonsterKind::Orc:    name = "orc";    break;
    }
    char buf[96];
    SDL_snprintf(buf, sizeof(buf), "The %s hits you for %d.", name, dmg);
    hud_.addMessage(buf);

    if (!player_->alive())
    {
        hud_.addMessage("You have been slain.");
        gameOver_ = true;
    }
}

void Game::enemyTurns()
{
    for (auto& e : enemies_)
    {
        if (!e->alive()) continue;

        Point ep = e->position();
        Point pp = player_->position();
        int   sight = statsFor(e->kind()).sightRange;

        if (!FOV::hasLineOfSight(map_, ep, pp)) continue;
        int distX = std::abs(ep.x - pp.x);
        int distY = std::abs(ep.y - pp.y);
        if (distX > sight || distY > sight) continue;

        if (distX + distY == 1)
        {
            enemyAttack(*e);
            if (gameOver_) return;
            continue;
        }

        auto path = Pathfind::astar(map_, ep, pp);
        if (path.empty()) continue;
        Point nextStep = path.front();
        if (nextStep == pp)        continue;
        if (enemyAt(nextStep))     continue;
        e->setPosition(nextStep);
    }

    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e){ return !e->alive(); }),
        enemies_.end());
}

void Game::doMagicMapping()
{
    for (int y = 0; y < map_.height(); ++y)
        for (int x = 0; x < map_.width(); ++x)
            map_.at(x, y).explored = true;
    sounds_.castScroll.play();
    hud_.addMessage("The map of this level is revealed!");
}

void Game::castFireballAt(Point center)
{
    sounds_.castScroll.play();
    hud_.addMessage("A fireball blooms.");

    for (int dy = -FIREBALL_RADIUS; dy <= FIREBALL_RADIUS; ++dy)
    {
        for (int dx = -FIREBALL_RADIUS; dx <= FIREBALL_RADIUS; ++dx)
        {
            Point p { center.x + dx, center.y + dy };
            if (!map_.inBounds(p.x, p.y)) continue;
            // Manhattan-ish circle; |dx|+|dy| <= radius gives a diamond.
            if (std::abs(dx) + std::abs(dy) > FIREBALL_RADIUS) continue;

            flashes_.add(p, Palette::FLASH_RED, FLASH_MS, nowMs());

            // Damage any enemy on this tile.
            if (Enemy* e = enemyAt(p))
            {
                e->takeDamage(FIREBALL_DAMAGE);
                if (!e->alive()) sounds_.kill.play();
            }
        }
    }

    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e){ return !e->alive(); }),
        enemies_.end());
}

void Game::save()
{
    if (SaveLoad::save(SAVE_PATH, map_, *player_, enemies_, items_, depth_))
        hud_.addMessage("Game saved.");
    else
        hud_.addMessage("Save FAILED.");
    dirty_ = true;
}

void Game::load()
{
    if (SaveLoad::load(SAVE_PATH, map_, *player_, enemies_, items_, depth_))
    {
        gameOver_ = false;
        recomputeFov();
        hud_.addMessage("Game loaded.");
    }
    else
    {
        hud_.addMessage("Load FAILED.");
    }
    dirty_ = true;
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer_,
        Palette::BG.r, Palette::BG.g, Palette::BG.b, 255);
    SDL_RenderClear(renderer_);

    // Walk the stack bottom-to-top so modal states paint on top.
    for (auto& s : stack_)
        s->render(*this, renderer_);

    SDL_RenderPresent(renderer_);
}

void Game::run()
{
    while (!quit_)
    {
        if (dirty_)
        {
            render();
            dirty_ = false;
        }

        SDL_Event ev;
        const Uint64 t0 = nowMs();
        const bool   animActive = flashes_.any(t0);

        // While animations are running, tick at ~60fps; otherwise idle.
        if (animActive)
        {
            if (!SDL_WaitEventTimeout(&ev, ANIMATION_TICK_MS))
            {
                // Timeout fired with no event — just re-render to advance
                // the fade-out animation.
                flashes_.prune(nowMs());
                dirty_ = true;
                continue;
            }
        }
        else
        {
            if (!SDL_WaitEvent(&ev)) break;
        }

        if (GameState* s = top())
        {
            InputResult r = s->handleEvent(*this, ev);
            if (r.tookTurn && !gameOver_)
            {
                // Only run enemy turns when we're back in pure gameplay
                // (the player isn't sitting inside an inventory popup).
                if (top() == nullptr) break;
                if (dynamic_cast<PlayingState*>(top()) != nullptr)
                    enemyTurns();
            }
            if (r.handled || ev.type == SDL_EVENT_WINDOW_EXPOSED)
                dirty_ = true;
        }

        flashes_.prune(nowMs());
        if (flashes_.any(nowMs())) dirty_ = true;
    }
}
