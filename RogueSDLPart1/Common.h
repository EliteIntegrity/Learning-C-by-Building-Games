#pragma once
#include <SDL3/SDL.h>

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

inline bool operator!=(const Point& a, const Point& b)
{
    return !(a == b);
}

// One ASCII cell on screen. Square = simpler FOV / pathfinding math.
constexpr int CELL_PX  = 16;

// Map dimensions in cells.
constexpr int MAP_W    = 80;
constexpr int MAP_H    = 45;

// Window dimensions in pixels.
constexpr int WINDOW_W = CELL_PX * MAP_W;   // 1280
constexpr int WINDOW_H = CELL_PX * MAP_H;   // 720

// How many tiles in every direction the player can see.
constexpr int FOV_RADIUS = 8;

// Standard palette.
namespace Palette
{
    constexpr SDL_Color BG          = {  10,  10,  16, 255 };
    constexpr SDL_Color WALL_VIS    = { 180, 160, 110, 255 };
    constexpr SDL_Color WALL_DARK   = {  60,  55,  40, 255 };
    constexpr SDL_Color FLOOR_VIS  =  { 110, 110, 130, 255 };
    constexpr SDL_Color FLOOR_DARK = {  40,  40,  55, 255 };
    constexpr SDL_Color STAIRS     =  { 240, 220,  80, 255 };
    constexpr SDL_Color PLAYER     =  { 255, 255, 255, 255 };
}
