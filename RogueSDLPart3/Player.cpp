#include "Player.h"
#include "Map.h"

Player::Player(Point start)
    : Entity(start, '@', Palette::PLAYER)
{
}

bool Player::tryMove(int dx, int dy, const Map& map)
{
    int nx = pos_.x + dx;
    int ny = pos_.y + dy;
    if (map.isBlocked(nx, ny)) return false;
    pos_.x = nx;
    pos_.y = ny;
    return true;
}

bool Player::addItem(ItemKind k)
{
    if ((int)inventory_.size() >= INVENTORY_CAPACITY) return false;
    inventory_.push_back(k);
    return true;
}

int Player::findSlot(ItemKind k) const
{
    for (int i = 0; i < (int)inventory_.size(); ++i)
        if (inventory_[i] == k) return i;
    return -1;
}

void Player::removeSlot(int index)
{
    if (index < 0 || index >= (int)inventory_.size()) return;
    inventory_.erase(inventory_.begin() + index);
}

Player::UseResult Player::useSlot(int index)
{
    UseResult r;
    if (index < 0 || index >= (int)inventory_.size()) return r;

    ItemKind k = inventory_[index];
    r.kind = k;

    if (k == ItemKind::Potion)
    {
        heal(8);
        removeSlot(index);
        r.used = true;
        return r;
    }
    if (k == ItemKind::ScrollMagicMap)
    {
        // Caller handles the world-side effect; we just consume.
        removeSlot(index);
        r.used = true;
        return r;
    }
    if (k == ItemKind::ScrollFireball)
    {
        // Don't consume yet — caller will after targeting confirms.
        r.needsAim = true;
        return r;
    }
    if (isWeapon(k))
    {
        ItemKind previous = wielded_;
        wielded_ = k;
        removeSlot(index);
        if (previous != ItemKind::None)
            inventory_.push_back(previous);   // swap, don't destroy
        r.used = true;
        return r;
    }
    if (k == ItemKind::Gold)
    {
        // Gold isn't really an inventory item, but defensively handle it.
        gold_ += 1;
        removeSlot(index);
        r.used = true;
        return r;
    }

    return r;
}

void Player::resetForNewGame(Point start)
{
    pos_       = start;
    hp_        = 20;
    maxHp_     = 20;
    damage_    = 4;
    gold_      = 0;
    wielded_   = ItemKind::None;
    inventory_.clear();
}
