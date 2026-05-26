#pragma once

class Map;
class Player;

// ============================================================
//  SaveLoad  -  plain-text save format.
// ============================================================
//
//  Format is deliberately human-readable so you can open the
//  file in any text editor when something goes wrong. Lines
//  are header keys followed by data:
//
//      ROGUE_SDL_SAVE 1
//      WIDTH 80
//      HEIGHT 45
//      PLAYER 12 14
//      MAP
//      ##########
//      #........#
//      ...
//      ENDMAP
//
//  Tile encoding inside MAP:
//      '#' = unseen wall            '%' = explored wall
//      '.' = unseen floor           ',' = explored floor
//      '>' = unseen stairs          '<' = explored stairs
//
//  Visibility is recomputed at load time from the player's
//  position, so we don't store the per-tile visible flag.
// ============================================================

namespace SaveLoad
{
    bool save(const char* path, const Map& map, const Player& player);
    bool load(const char* path, Map& map, Player& player);
}
