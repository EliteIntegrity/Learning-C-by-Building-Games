#include "PlayingState.h"
#include "InventoryState.h"
#include "Game.h"

InputResult PlayingState::handleEvent(Game& game, const SDL_Event& ev)
{
    InputResult r;

    if (ev.type == SDL_EVENT_QUIT) { game.requestQuit(); return r; }
    if (ev.type != SDL_EVENT_KEY_DOWN) return r;
    r.handled = true;

    if (ev.key.scancode == SDL_SCANCODE_ESCAPE) { game.requestQuit(); return r; }

    // Game-over input: only R restarts.
    if (game.gameOver())
    {
        if (ev.key.scancode == SDL_SCANCODE_R)
        {
            game.newGame();
            r.tookTurn = true;
        }
        return r;
    }

    int dx = 0, dy = 0;
    switch (ev.key.scancode)
    {
        case SDL_SCANCODE_W: case SDL_SCANCODE_UP:    dy = -1; break;
        case SDL_SCANCODE_S: case SDL_SCANCODE_DOWN:  dy = +1; break;
        case SDL_SCANCODE_A: case SDL_SCANCODE_LEFT:  dx = -1; break;
        case SDL_SCANCODE_D: case SDL_SCANCODE_RIGHT: dx = +1; break;

        case SDL_SCANCODE_PERIOD:
            if (game.map().at(game.player().position().x,
                              game.player().position().y).terrain
                == Terrain::StairsDown)
            {
                game.descend();
                r.tookTurn = true;
            }
            return r;

        case SDL_SCANCODE_I:
            game.pushState(std::make_unique<InventoryState>());
            return r;

        case SDL_SCANCODE_F5: game.save(); return r;
        case SDL_SCANCODE_F9: game.load(); return r;

        default: return r;
    }
    if (dx == 0 && dy == 0) return r;

    Point dest { game.player().position().x + dx,
                 game.player().position().y + dy };

    if (Enemy* target = game.enemyAt(dest))
    {
        game.playerAttack(*target);
        r.tookTurn = true;
        return r;
    }

    if (game.player().tryMove(dx, dy, game.map()))
    {
        if (Item* it = game.itemAt(game.player().position()))
        {
            char buf[96];
            switch (it->kind())
            {
                case ItemKind::Potion:
                case ItemKind::ScrollMagicMap:
                case ItemKind::ScrollFireball:
                case ItemKind::WeaponDagger:
                case ItemKind::WeaponSword:
                case ItemKind::WeaponHammer:
                    if (game.player().addItem(it->kind()))
                    {
                        game.sounds().pickup.play();
                        SDL_snprintf(buf, sizeof(buf), "You picked up: %s.",
                                     itemDisplayName(it->kind()));
                        game.hud().addMessage(buf);
                    }
                    else
                    {
                        game.hud().addMessage("Your inventory is full.");
                        // Item stays on the floor.
                        return r;
                    }
                    break;

                case ItemKind::Gold:
                    game.player().addGold(it->amount());
                    game.sounds().pickup.play();
                    SDL_snprintf(buf, sizeof(buf),
                                 "You picked up %d gold.", it->amount());
                    game.hud().addMessage(buf);
                    break;

                default: break;
            }
            game.items().erase(
                std::remove_if(game.items().begin(), game.items().end(),
                    [&](const std::unique_ptr<Item>& e){
                        return e.get() == it;
                    }),
                game.items().end());
        }
        game.recomputeFov();
        r.tookTurn = true;
    }
    return r;
}

void PlayingState::render(Game& game, SDL_Renderer* renderer)
{
    game.map().render(renderer, game.glyphs());

    // Items on visible tiles only.
    for (const auto& it : game.items())
    {
        Point p = it->position();
        if (game.map().at(p.x, p.y).visible)
            it->render(renderer, game.glyphs());
    }
    for (const auto& e : game.enemies())
    {
        Point p = e->position();
        if (game.map().at(p.x, p.y).visible)
            e->render(renderer, game.glyphs());
    }
    game.player().render(renderer, game.glyphs());

    game.flashes().render(renderer, game.nowMs());

    game.hud().render(renderer, game.glyphs(), game.player(),
                      game.depth(), game.gameOver());
}
