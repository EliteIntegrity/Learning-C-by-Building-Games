#include "SaveLoad.h"
#include "Map.h"
#include "Tile.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "Common.h"

#include <fstream>
#include <string>

namespace
{
    char encode(const Tile& t)
    {
        switch (t.terrain)
        {
            case Terrain::Wall:       return t.explored ? '%' : '#';
            case Terrain::Floor:      return t.explored ? ',' : '.';
            case Terrain::StairsDown: return t.explored ? '<' : '>';
        }
        return '?';
    }
    void decode(char ch, Tile& t)
    {
        switch (ch)
        {
            case '#': t.terrain = Terrain::Wall;       t.explored = false; break;
            case '%': t.terrain = Terrain::Wall;       t.explored = true;  break;
            case '.': t.terrain = Terrain::Floor;      t.explored = false; break;
            case ',': t.terrain = Terrain::Floor;      t.explored = true;  break;
            case '>': t.terrain = Terrain::StairsDown; t.explored = false; break;
            case '<': t.terrain = Terrain::StairsDown; t.explored = true;  break;
            default:  t.terrain = Terrain::Wall;       t.explored = false; break;
        }
        t.visible = false;
    }
}

namespace SaveLoad
{
    bool save(const char* path,
              const Map& map,
              const Player& player,
              const std::vector<std::unique_ptr<Enemy>>& enemies,
              const std::vector<std::unique_ptr<Item>>&  items,
              int depth)
    {
        std::ofstream f(path);
        if (!f) return false;

        f << "ROGUE_SDL_SAVE 3\n";
        f << "WIDTH "  << map.width()  << "\n";
        f << "HEIGHT " << map.height() << "\n";
        f << "DEPTH "  << depth << "\n";
        f << "PLAYER " << player.position().x << " " << player.position().y
                       << " " << player.hp()  << " " << player.maxHp()
                       << " " << player.gold()
                       << " " << (int)player.wielded() << "\n";

        for (ItemKind k : player.inventory())
            f << "INV " << (int)k << "\n";

        for (auto& e : enemies)
        {
            if (!e->alive()) continue;
            f << "ENEMY " << (int)e->kind() << " "
                          << e->position().x << " "
                          << e->position().y << " "
                          << e->hp() << "\n";
        }
        for (auto& it : items)
        {
            f << "ITEM " << (int)it->kind() << " "
                         << it->position().x << " "
                         << it->position().y << " "
                         << it->amount() << "\n";
        }

        f << "MAP\n";
        for (int y = 0; y < map.height(); ++y)
        {
            for (int x = 0; x < map.width(); ++x)
                f << encode(map.at(x, y));
            f << "\n";
        }
        f << "ENDMAP\n";
        return (bool)f;
    }

    bool load(const char* path,
              Map& map,
              Player& player,
              std::vector<std::unique_ptr<Enemy>>& enemies,
              std::vector<std::unique_ptr<Item>>&  items,
              int& depth)
    {
        std::ifstream f(path);
        if (!f) return false;

        std::string token;
        f >> token;
        if (token != "ROGUE_SDL_SAVE") return false;
        int version = 0;
        f >> version;
        if (version != 3) return false;

        int w = 0, h = 0;
        Point ppos { 0, 0 };
        int php = 20, pmax = 20, pgold = 0;
        int pwielded = (int)ItemKind::None;
        std::vector<ItemKind> inv;
        enemies.clear();
        items.clear();

        while (f >> token)
        {
            if (token == "WIDTH")       f >> w;
            else if (token == "HEIGHT") f >> h;
            else if (token == "DEPTH")  f >> depth;
            else if (token == "PLAYER")
            {
                f >> ppos.x >> ppos.y >> php >> pmax >> pgold >> pwielded;
            }
            else if (token == "INV")
            {
                int k = 0;
                f >> k;
                inv.push_back((ItemKind)k);
            }
            else if (token == "ENEMY")
            {
                int k = 0, ex = 0, ey = 0, ehp = 0;
                f >> k >> ex >> ey >> ehp;
                auto e = std::make_unique<Enemy>(Point{ ex, ey }, (MonsterKind)k);
                int adjust = e->hp() - ehp;
                if (adjust > 0) e->takeDamage(adjust);
                enemies.push_back(std::move(e));
            }
            else if (token == "ITEM")
            {
                int k = 0, ix = 0, iy = 0, amount = 0;
                f >> k >> ix >> iy >> amount;
                items.push_back(std::make_unique<Item>(
                    Point{ ix, iy }, (ItemKind)k, amount));
            }
            else if (token == "MAP") break;
        }
        if (w != map.width() || h != map.height()) return false;

        f.get();
        for (int y = 0; y < h; ++y)
        {
            std::string row;
            if (!std::getline(f, row)) return false;
            for (int x = 0; x < w && x < (int)row.size(); ++x)
                decode(row[x], map.at(x, y));
        }

        player.resetForNewGame(ppos);
        if (php < player.maxHp())
            player.takeDamage(player.maxHp() - php);
        player.addGold(pgold);
        player.wield((ItemKind)pwielded);
        for (ItemKind k : inv) player.addItem(k);

        return true;
    }
}
