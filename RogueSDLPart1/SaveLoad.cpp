#include "SaveLoad.h"
#include "Map.h"
#include "Tile.h"
#include "Player.h"
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
    bool save(const char* path, const Map& map, const Player& player)
    {
        std::ofstream f(path);
        if (!f) return false;

        f << "ROGUE_SDL_SAVE 1\n";
        f << "WIDTH "  << map.width()  << "\n";
        f << "HEIGHT " << map.height() << "\n";
        f << "PLAYER " << player.position().x << " " << player.position().y << "\n";
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

    bool load(const char* path, Map& map, Player& player)
    {
        std::ifstream f(path);
        if (!f) return false;

        std::string token;

        // Header
        f >> token;
        if (token != "ROGUE_SDL_SAVE") return false;
        int version = 0;
        f >> version;
        if (version != 1) return false;

        int w = 0, h = 0;
        Point ppos { 0, 0 };

        // Read key/value pairs until we hit MAP
        while (f >> token)
        {
            if (token == "WIDTH")   f >> w;
            else if (token == "HEIGHT") f >> h;
            else if (token == "PLAYER") f >> ppos.x >> ppos.y;
            else if (token == "MAP") break;
            else { /* skip unknown key */ }
        }

        if (w != map.width() || h != map.height()) return false;

        // Read map rows
        f.get();   // eat the newline after "MAP"
        for (int y = 0; y < h; ++y)
        {
            std::string row;
            if (!std::getline(f, row)) return false;
            for (int x = 0; x < w && x < (int)row.size(); ++x)
                decode(row[x], map.at(x, y));
        }

        f >> token;   // should be ENDMAP, but we accept anything

        player.setPosition(ppos);
        return true;
    }
}
