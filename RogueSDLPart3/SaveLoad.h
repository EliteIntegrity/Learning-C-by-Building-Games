#pragma once
#include <memory>
#include <vector>

class Map;
class Player;
class Enemy;
class Item;

namespace SaveLoad
{
    bool save(const char* path,
              const Map& map,
              const Player& player,
              const std::vector<std::unique_ptr<Enemy>>& enemies,
              const std::vector<std::unique_ptr<Item>>&  items,
              int depth);

    bool load(const char* path,
              Map& map,
              Player& player,
              std::vector<std::unique_ptr<Enemy>>& enemies,
              std::vector<std::unique_ptr<Item>>&  items,
              int& depth);
}
