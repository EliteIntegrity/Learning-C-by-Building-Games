#pragma once
#include <vector>
#include "Entity.h"
#include "Item.h"

class Map;

// ============================================================
//  Player
// ============================================================
//
//  Adds an inventory (a vector of ItemKind) and a single
//  wielded-weapon slot. Effective damage = base + weapon bonus.
// ============================================================

class Player : public Entity
{
public:
    explicit Player(Point start);

    bool tryMove(int dx, int dy, const Map& map);

    int  hp()       const { return hp_; }
    int  maxHp()    const { return maxHp_; }
    int  baseDmg()  const { return damage_; }
    int  effectiveDamage() const { return damage_ + weaponDamageBonus(wielded_); }
    int  gold()     const { return gold_; }
    bool alive()    const { return hp_ > 0; }

    ItemKind wielded() const { return wielded_; }

    const std::vector<ItemKind>& inventory() const { return inventory_; }

    void takeDamage(int amount)   { hp_ -= amount; if (hp_ < 0) hp_ = 0; }
    void heal(int amount)         { hp_ += amount; if (hp_ > maxHp_) hp_ = maxHp_; }
    void addGold(int amount)      { gold_ += amount; }

    // Pickup. Returns false if inventory is full.
    bool addItem(ItemKind k);

    // Use the item at slot `index`. Returns:
    //   used      — true if the item was consumed (or wielded)
    //   needsAim  — true if the action needs a target (caller should push TargetingState)
    //   bonusInfo — slot info for the caller (e.g., scroll kind to remember while aiming)
    struct UseResult
    {
        bool     used     = false;
        bool     needsAim = false;
        ItemKind kind     = ItemKind::None;
    };
    UseResult useSlot(int index);

    // Remove an item from inventory by slot (used after the targeting
    // confirms / cancels).
    void removeSlot(int index);

    // Find the slot of a given kind (-1 if not present).
    int  findSlot(ItemKind k) const;

    void wield(ItemKind weapon) { wielded_ = weapon; }

    void resetForNewGame(Point start);

private:
    int hp_      = 20;
    int maxHp_   = 20;
    int damage_  = 4;
    int gold_    = 0;

    ItemKind              wielded_   = ItemKind::None;
    std::vector<ItemKind> inventory_;
};
