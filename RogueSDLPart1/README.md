# Rogue SDL — Chapter 28 (Part 1)

In-book project for Chapter 28 of *Learning C++ by Building Games*. This
folder is the state of the game at the **end of Chapter 28**:

- Procedurally-generated BSP dungeon (80×45 cells)
- ASCII rendering via a pre-built `GlyphCache`
- Turn-based main loop that uses `SDL_WaitEvent` (zero CPU between turns)
- Player movement with WASD or arrow keys
- Field of view (Bresenham line-of-sight, radius 8)
- Stairs descend to a freshly generated dungeon
- Save/load to `rogue_sdl_save.txt` (plain text)

The Chapter 29 version (combat, enemies with A*, items, multi-level
descent, message log) lives in the sibling **Rogue SDL Part 2** folder.

## Controls

- **WASD / arrow keys** — move one tile
- **.** (period) — descend the stairs (when standing on `>`)
- **F5** — save
- **F9** — load
- **Esc** — quit

## To build

You will need **SDL 3** and **SDL3_ttf**. Both are free downloads from
libsdl.org. Set up the project as in Chapter 1, then also:

1. Add `SDL3_ttf.lib` to **Linker > Input > Additional Dependencies**.
2. Copy `SDL3_ttf.dll` next to your `.exe` (in `x64\Debug`).
3. Add the SDL3_ttf `include` and `lib\x64` paths.

The font file `RobotoMono-Light.ttf` lives in the SDL3 Projects root —
copy it into your project's working directory (next to the `.exe`).

## File layout

| File                | Job                                       |
|---------------------|-------------------------------------------|
| `main.cpp`          | SDL + TTF init, hand off to `Game`        |
| `Common.h`          | `Point`, constants, palette               |
| `Tile.h`            | `Tile` struct, `Terrain` enum             |
| `Map.h/cpp`         | The grid; render; bounds/blocked queries  |
| `MapGen.h/cpp`      | BSP procedural dungeon generator          |
| `GlyphCache.h/cpp`  | Per-character pre-baked SDL textures      |
| `Entity.h/cpp`      | Base class for anything on a tile         |
| `Player.h/cpp`      | The `@` — derives from Entity             |
| `FOV.h/cpp`         | Bresenham line-of-sight visibility        |
| `SaveLoad.h/cpp`    | Plain-text serialise/deserialise          |
| `Game.h/cpp`        | Top-level game state and main loop        |
