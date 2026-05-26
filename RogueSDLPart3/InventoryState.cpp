#include "InventoryState.h"
#include "TargetingState.h"
#include "Game.h"

#include <cstdio>

namespace
{
    // Slot letter (a, b, c, ...) for a 0-based slot index.
    char slotLetter(int i) { return (char)('a' + i); }
}

InputResult InventoryState::handleEvent(Game& game, const SDL_Event& ev)
{
    InputResult r;
    if (ev.type != SDL_EVENT_KEY_DOWN) return r;
    r.handled = true;

    if (ev.key.scancode == SDL_SCANCODE_ESCAPE ||
        ev.key.scancode == SDL_SCANCODE_I)
    {
        game.popState();
        return r;
    }

    // a..l -> slot 0..11
    int slot = -1;
    if (ev.key.scancode >= SDL_SCANCODE_A &&
        ev.key.scancode <= SDL_SCANCODE_Z)
    {
        slot = ev.key.scancode - SDL_SCANCODE_A;
    }
    if (slot < 0 || slot >= INVENTORY_CAPACITY) return r;
    if (slot >= (int)game.player().inventory().size()) return r;

    Player::UseResult ur = game.player().useSlot(slot);
    if (!ur.used && !ur.needsAim)
    {
        game.hud().addMessage("You can't use that.");
        return r;
    }

    if (ur.needsAim)
    {
        // Fireball — pop the inventory, then push targeting.
        game.popState();
        game.pushState(std::make_unique<TargetingState>(
            TargetAction::Fireball, slot, game.player().position()));
        return r;   // no turn yet — wait for confirm
    }

    // Used / wielded / quaffed — log + take a turn.
    char buf[96];
    switch (ur.kind)
    {
        case ItemKind::Potion:
            game.sounds().quaff.play();
            game.hud().addMessage("You quaff a potion. (+8 HP)");
            break;
        case ItemKind::ScrollMagicMap:
            game.doMagicMapping();
            break;
        case ItemKind::WeaponDagger:
        case ItemKind::WeaponSword:
        case ItemKind::WeaponHammer:
            SDL_snprintf(buf, sizeof(buf), "You wield the %s.",
                         itemDisplayName(ur.kind));
            game.hud().addMessage(buf);
            break;
        default:
            break;
    }

    game.popState();
    r.tookTurn = true;
    return r;
}

void InventoryState::render(Game& game, SDL_Renderer* renderer)
{
    // First, render the state(s) below us so the world is still visible
    // behind the popup.
    // (Game::render walks the stack bottom-to-top, so the state(s) below
    // have already been drawn before our render() was called.)

    // Now a dim overlay across the full window.
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer,
        Palette::OVERLAY_DIM.r, Palette::OVERLAY_DIM.g,
        Palette::OVERLAY_DIM.b, Palette::OVERLAY_DIM.a);
    SDL_FRect full { 0, 0, (float)WINDOW_W, (float)WINDOW_H };
    SDL_RenderFillRect(renderer, &full);

    // Popup: a centered box of ~50 cols x 16 rows.
    const int popW = 50;
    const int popH = 16;
    const int popX = (MAP_W - popW) / 2;
    const int popY = 2;

    const auto& glyphs = game.glyphs();
    glyphs.drawText(renderer, "--- Inventory ---", popX, popY, Palette::HUD_TEXT);

    const auto& inv = game.player().inventory();
    if (inv.empty())
    {
        glyphs.drawText(renderer, "(empty)", popX + 2, popY + 2, Palette::HUD_DIM);
    }
    else
    {
        for (size_t i = 0; i < inv.size() && (int)i < popH - 4; ++i)
        {
            char line[96];
            SDL_snprintf(line, sizeof(line), "%c) %s",
                         slotLetter((int)i), itemDisplayName(inv[i]));
            glyphs.drawText(renderer, line, popX + 2, popY + 2 + (int)i,
                            Palette::HUD_TEXT);
        }
    }

    glyphs.drawText(renderer,
        "letter: use     Esc: close",
        popX, popY + popH - 1, Palette::HUD_DIM);
}
