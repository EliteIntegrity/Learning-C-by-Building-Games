#pragma once
#include <SDL3/SDL.h>
#include <functional>

struct Point
{
    int x = 0;
    int y = 0;
};

inline bool operator==(const Point& a, const Point& b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(const Point& a, const Point& b) { return !(a == b); }

namespace std
{
    template<> struct hash<Point>
    {
        size_t operator()(const Point& p) const noexcept
        {
            return (size_t)p.x * 73856093u ^ (size_t)p.y * 19349663u;
        }
    };
}

constexpr int CELL_PX      = 16;
constexpr int MAP_W        = 80;
constexpr int MAP_H        = 40;
constexpr int HUD_ROWS     = 5;
constexpr int WINDOW_W     = CELL_PX * MAP_W;
constexpr int WINDOW_H     = CELL_PX * (MAP_H + HUD_ROWS);
constexpr int FOV_RADIUS   = 8;
constexpr int MAX_MESSAGES = HUD_ROWS - 1;

constexpr int INVENTORY_CAPACITY = 12;
constexpr int FIREBALL_RADIUS    = 2;
constexpr int FIREBALL_DAMAGE    = 8;
constexpr int FLASH_MS           = 90;     // how long a hit flash lasts
constexpr int ANIMATION_TICK_MS  = 16;     // ~60fps during animations

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

    constexpr SDL_Color ITEM_POTION   = { 220,  80, 220, 255 };
    constexpr SDL_Color ITEM_GOLD     = { 240, 220,  80, 255 };
    constexpr SDL_Color ITEM_SCROLL   = { 180, 220, 255, 255 };
    constexpr SDL_Color ITEM_WEAPON   = { 200, 200, 220, 255 };

    constexpr SDL_Color HUD_TEXT     = { 200, 200, 210, 255 };
    constexpr SDL_Color HUD_DIM      = { 100, 100, 110, 255 };
    constexpr SDL_Color HUD_BAD      = { 240,  80,  80, 255 };
    constexpr SDL_Color HUD_GOOD     = { 100, 220, 130, 255 };

    constexpr SDL_Color FLASH_RED    = { 255,  60,  60, 255 };
    constexpr SDL_Color TARGET_CURSOR= { 255, 230,  80, 255 };
    constexpr SDL_Color OVERLAY_DIM  = {   0,   0,   0, 170 };   // alpha blend
}
