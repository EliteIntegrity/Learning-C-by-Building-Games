#pragma once
#include <SDL3/SDL.h>
#include <functional>      // std::hash

// ============================================================
//  Common  -  shared types and constants for Rogue SDL.
// ============================================================

struct Point
{
    int x = 0;
    int y = 0;
};

inline bool operator==(const Point& a, const Point& b)
{
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const Point& a, const Point& b) { return !(a == b); }

// std::hash specialisation so Point can be a key in std::unordered_map
// (used by the A* pathfinder).
namespace std
{
    template<> struct hash<Point>
    {
        size_t operator()(const Point& p) const noexcept
        {
            // Cheap mix — collision-light for our small grids.
            return (size_t)p.x * 73856093u ^ (size_t)p.y * 19349663u;
        }
    };
}

constexpr int CELL_PX    = 16;
constexpr int MAP_W      = 80;
constexpr int MAP_H      = 40;     // Chapter 28 used 45; we trim 5 rows for the HUD.
constexpr int HUD_ROWS   = 5;
constexpr int WINDOW_W   = CELL_PX * MAP_W;                 // 1280
constexpr int WINDOW_H   = CELL_PX * (MAP_H + HUD_ROWS);    // 720
constexpr int FOV_RADIUS = 8;
constexpr int MAX_MESSAGES = HUD_ROWS - 1;

namespace Palette
{
    constexpr SDL_Color BG          = {  10,  10,  16, 255 };
    constexpr SDL_Color WALL_VIS    = { 180, 160, 110, 255 };
    constexpr SDL_Color WALL_DARK   = {  60,  55,  40, 255 };
    constexpr SDL_Color FLOOR_VIS   = { 110, 110, 130, 255 };
    constexpr SDL_Color FLOOR_DARK  = {  40,  40,  55, 255 };
    constexpr SDL_Color STAIRS      = { 240, 220,  80, 255 };
    constexpr SDL_Color PLAYER      = { 255, 255, 255, 255 };

    constexpr SDL_Color ENEMY_RAT    = { 180, 180, 100, 255 };
    constexpr SDL_Color ENEMY_GOBLIN = { 100, 220, 100, 255 };
    constexpr SDL_Color ENEMY_ORC    = { 220, 100, 100, 255 };

    constexpr SDL_Color ITEM_POTION  = { 220,  80, 220, 255 };
    constexpr SDL_Color ITEM_GOLD    = { 240, 220,  80, 255 };

    constexpr SDL_Color HUD_TEXT     = { 200, 200, 210, 255 };
    constexpr SDL_Color HUD_HP       = { 220, 100, 100, 255 };
    constexpr SDL_Color HUD_DIM      = { 100, 100, 110, 255 };
    constexpr SDL_Color HUD_GOOD     = { 100, 220, 130, 255 };
    constexpr SDL_Color HUD_BAD      = { 240,  80,  80, 255 };
}
