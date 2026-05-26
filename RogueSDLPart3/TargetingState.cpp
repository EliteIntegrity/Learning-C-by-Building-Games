#include "TargetingState.h"
#include "Game.h"

#include <cstdio>

TargetingState::TargetingState(TargetAction action, int consumeSlot, Point startCursor)
    : action_(action), consumeSlot_(consumeSlot), cursor_(startCursor)
{
}

void TargetingState::onEnter(Game& game)
{
    game.hud().addMessage("Choose a target. Enter to confirm, Esc to cancel.");
}

InputResult TargetingState::handleEvent(Game& game, const SDL_Event& ev)
{
    InputResult r;
    if (ev.type != SDL_EVENT_KEY_DOWN) return r;
    r.handled = true;

    int dx = 0, dy = 0;
    switch (ev.key.scancode)
    {
        case SDL_SCANCODE_ESCAPE:
            game.hud().addMessage("You stop concentrating.");
            game.popState();
            return r;

        case SDL_SCANCODE_RETURN:
        case SDL_SCANCODE_KP_ENTER:
            // Execute and consume.
            if (action_ == TargetAction::Fireball)
            {
                game.castFireballAt(cursor_);
                game.player().removeSlot(consumeSlot_);
            }
            game.popState();
            r.tookTurn = true;
            return r;

        case SDL_SCANCODE_W: case SDL_SCANCODE_UP:    dy = -1; break;
        case SDL_SCANCODE_S: case SDL_SCANCODE_DOWN:  dy = +1; break;
        case SDL_SCANCODE_A: case SDL_SCANCODE_LEFT:  dx = -1; break;
        case SDL_SCANCODE_D: case SDL_SCANCODE_RIGHT: dx = +1; break;

        default: return r;
    }
    cursor_.x += dx;
    cursor_.y += dy;
    if (cursor_.x < 0)            cursor_.x = 0;
    if (cursor_.y < 0)            cursor_.y = 0;
    if (cursor_.x >= MAP_W)       cursor_.x = MAP_W - 1;
    if (cursor_.y >= MAP_H)       cursor_.y = MAP_H - 1;
    return r;
}

void TargetingState::render(Game& game, SDL_Renderer* renderer)
{
    // The state below us already painted the world. We add a cursor and
    // a preview of the fireball blast area.

    if (action_ == TargetAction::Fireball)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 80, 0, 60);
        for (int dy = -FIREBALL_RADIUS; dy <= FIREBALL_RADIUS; ++dy)
        {
            for (int dx = -FIREBALL_RADIUS; dx <= FIREBALL_RADIUS; ++dx)
            {
                if (std::abs(dx) + std::abs(dy) > FIREBALL_RADIUS) continue;
                Point p { cursor_.x + dx, cursor_.y + dy };
                if (p.x < 0 || p.y < 0 || p.x >= MAP_W || p.y >= MAP_H) continue;
                SDL_FRect r {
                    (float)(p.x * CELL_PX),
                    (float)(p.y * CELL_PX),
                    (float)CELL_PX, (float)CELL_PX
                };
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    // Cursor glyph on top.
    game.glyphs().draw(renderer, 'X', cursor_.x, cursor_.y, Palette::TARGET_CURSOR);
}
