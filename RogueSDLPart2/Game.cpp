#include "Game.h"
#include "FOV.h"
#include "Pathfind.h"
#include "SaveLoad.h"

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
}

void Game::newGame()
{
    depth_    = 1;
    gameOver_ = false;
    hud_.clear();

    Point start = gen_.generate(map_, enemies_, items_, depth_);

    player_ = std::make_unique<Player>(start);
    player_->resetForNewGame(start);

    recomputeFov();
    hud_.addMessage("Welcome to Rogue SDL! Find the > to descend.");
    dirty_ = true;
}

void Game::descend()
{
    ++depth_;
    Point start = gen_.generate(map_, enemies_, items_, depth_);
    player_->setPosition(start);
    recomputeFov();
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
    int dmg = player_->damage();
    target.takeDamage(dmg);

    char buf[96];
    const char* name = "enemy";
    switch (target.kind())
    {
        case MonsterKind::Rat:    name = "rat";    break;
        case MonsterKind::Goblin: name = "goblin"; break;
        case MonsterKind::Orc:    name = "orc";    break;
    }
    if (target.alive())
    {
        SDL_snprintf(buf, sizeof(buf), "You hit the %s for %d damage.", name, dmg);
        hud_.addMessage(buf);
    }
    else
    {
        SDL_snprintf(buf, sizeof(buf), "You kill the %s!", name);
        hud_.addMessage(buf);
    }
}

void Game::enemyAttack(Enemy& attacker)
{
    int dmg = attacker.damage();
    player_->takeDamage(dmg);

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

        int sight = statsFor(e->kind()).sightRange;

        // Only chase if we can actually see the player.
        if (!FOV::hasLineOfSight(map_, ep, pp)) continue;
        int distX = std::abs(ep.x - pp.x);
        int distY = std::abs(ep.y - pp.y);
        if (distX > sight || distY > sight) continue;

        // Adjacent? Attack instead of moving.
        if (distX + distY == 1)
        {
            enemyAttack(*e);
            if (gameOver_) return;
            continue;
        }

        // Otherwise A* a step toward the player.
        auto path = Pathfind::astar(map_, ep, pp);
        if (path.empty()) continue;

        Point nextStep = path.front();
        // Don't step onto another enemy or the player's tile.
        if (nextStep == pp) continue;
        if (enemyAt(nextStep)) continue;

        e->setPosition(nextStep);
    }

    // Clear out any dead enemies.
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [](const std::unique_ptr<Enemy>& e){ return !e->alive(); }),
        enemies_.end());
}

bool Game::handleEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_EVENT_QUIT) { quit_ = true; return false; }
    if (ev.type != SDL_EVENT_KEY_DOWN) return false;

    if (ev.key.scancode == SDL_SCANCODE_ESCAPE) { quit_ = true; return false; }

    // Game-over input: only R restarts.
    if (gameOver_)
    {
        if (ev.key.scancode == SDL_SCANCODE_R)
        {
            newGame();
            return true;
        }
        return false;
    }

    int dx = 0, dy = 0;
    switch (ev.key.scancode)
    {
        case SDL_SCANCODE_W: case SDL_SCANCODE_UP:    dy = -1; break;
        case SDL_SCANCODE_S: case SDL_SCANCODE_DOWN:  dy = +1; break;
        case SDL_SCANCODE_A: case SDL_SCANCODE_LEFT:  dx = -1; break;
        case SDL_SCANCODE_D: case SDL_SCANCODE_RIGHT: dx = +1; break;

        case SDL_SCANCODE_PERIOD:
            if (map_.at(player_->position().x,
                        player_->position().y).terrain == Terrain::StairsDown)
            {
                descend();
                return true;
            }
            return false;

        case SDL_SCANCODE_H:
            if (player_->usePotion())
            {
                hud_.addMessage("You quaff a potion. (+8 HP)");
                return true;
            }
            hud_.addMessage("You have no potions.");
            dirty_ = true;
            return false;

        case SDL_SCANCODE_F5: save(); return false;
        case SDL_SCANCODE_F9: load(); return false;

        default: return false;
    }
    if (dx == 0 && dy == 0) return false;

    Point dest { player_->position().x + dx, player_->position().y + dy };

    // Enemy on the destination? Bump-attack.
    if (Enemy* target = enemyAt(dest))
    {
        playerAttack(*target);
        // Player turn used regardless of kill or miss.
        return true;
    }

    // Empty floor? Step. Also pick up any item on that tile.
    if (player_->tryMove(dx, dy, map_))
    {
        if (Item* it = itemAt(player_->position()))
        {
            char buf[96];
            switch (it->kind())
            {
                case ItemKind::Potion:
                    player_->addPotion();
                    hud_.addMessage("You picked up a potion.");
                    break;
                case ItemKind::Gold:
                    player_->addGold(it->amount());
                    SDL_snprintf(buf, sizeof(buf),
                                 "You picked up %d gold.", it->amount());
                    hud_.addMessage(buf);
                    break;
            }
            // Remove the item.
            items_.erase(
                std::remove_if(items_.begin(), items_.end(),
                    [&](const std::unique_ptr<Item>& e){
                        return e.get() == it;
                    }),
                items_.end());
        }
        recomputeFov();
        return true;
    }

    return false;
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer_,
        Palette::BG.r, Palette::BG.g, Palette::BG.b, 255);
    SDL_RenderClear(renderer_);

    map_.render(renderer_, glyphs_);

    // Draw items first, then enemies, then the player on top.
    for (const auto& it : items_)
    {
        Point p = it->position();
        if (map_.at(p.x, p.y).visible)
            it->render(renderer_, glyphs_);
    }
    for (const auto& e : enemies_)
    {
        Point p = e->position();
        if (map_.at(p.x, p.y).visible)
            e->render(renderer_, glyphs_);
    }
    if (player_) player_->render(renderer_, glyphs_);

    hud_.render(renderer_, glyphs_, *player_, depth_, gameOver_);

    SDL_RenderPresent(renderer_);
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
        if (!SDL_WaitEvent(&ev)) break;

        bool tookTurn = handleEvent(ev);

        if (tookTurn && !gameOver_)
            enemyTurns();

        if (tookTurn || ev.type == SDL_EVENT_WINDOW_EXPOSED)
            dirty_ = true;
    }
}
