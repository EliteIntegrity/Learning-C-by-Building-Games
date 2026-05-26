# Rogue SDL — Chapter 30 (Part 3)

In-book project for Chapter 30 — the bonus polish-and-depth chapter on top
of the Chapter 29 game. Adds:

- **State stack** — `Playing`, `Inventory`, `Targeting` as polymorphic
  `GameState` subclasses. One input channel, modal popups, no
  if-spaghetti.
- **Inventory screen** — `i` opens a popup with letter-keyed item slots.
- **Weapons** — dagger (+2), sword (+5), warhammer (+8); wield from
  inventory; HUD shows what you have.
- **Scrolls** — Magic Mapping (instant level reveal) and Fireball
  (targeted area damage, 2-tile radius).
- **Targeting** — yellow cursor with arrow keys, preview of the
  fireball blast area, Enter to confirm / Esc to cancel.
- **Sound effects** — hits, kills, pickups, quaffs, scrolls, descend.
  Uses SDL audio streams directly (no SDL_mixer).
- **Hit flashes** — affected tiles flash red for ~90 ms with fade-out.
  Bridges turn-based with animation by using `SDL_WaitEventTimeout` only
  while flashes are active.
- **Save format v3** — handles inventory, wielded weapon, scrolls.

## Controls

- **WASD / arrows** — move (bump into enemies to attack)
- **i** — open inventory
- **letter a..l** (in inventory) — use that item (wield / quaff / read)
- **arrows + Enter / Esc** (while targeting) — aim / cast / cancel
- **.** — descend stairs
- **F5 / F9** — save / load
- **R** — restart after death
- **Esc** — quit (or close inventory / targeting)

## To build

Same as Parts 1 & 2 — SDL 3 + SDL3_ttf, `RobotoMono-Light.ttf` next to
the `.exe`.

**Optional sound files.** Drop these `.wav` files next to your `.exe`
for sound effects (missing files are silent, not fatal):

- `snd_hit.wav`     — sword/claw impact
- `snd_hurt.wav`    — player getting hit
- `snd_kill.wav`    — enemy death
- `snd_pickup.wav`  — picking up an item
- `snd_quaff.wav`   — drinking a potion
- `snd_cast.wav`    — reading a scroll
- `snd_descend.wav` — descending stairs
- `snd_levelup.wav` — reserved (unused for now)

Any short WAV works. freesound.org and opengameart.org have plenty.

## New files vs Part 2

| New                 | Job                                       |
|---------------------|-------------------------------------------|
| `GameState.h`       | Abstract base for the state stack         |
| `PlayingState.h/cpp`| The gameplay state (movement, combat)     |
| `InventoryState.h/cpp` | Modal inventory popup                  |
| `TargetingState.h/cpp` | Modal targeting cursor                 |
| `Sound.h/cpp`       | RAII wrapper around SDL audio streams     |
| `Flash.h/cpp`       | Short-lived per-tile flash effects        |

Changed from Part 2:

| File                | What changed                                |
|---------------------|---------------------------------------------|
| `Common.h`          | More palette colors; animation constants    |
| `Item.h/cpp`        | New scroll + weapon kinds; helper functions |
| `Player.h/cpp`      | Inventory vector, wielded weapon, useSlot   |
| `MapGen.cpp`        | Spawns scrolls + weapons                    |
| `Game.h/cpp`        | State stack; new actions; animation tick    |
| `HUD.cpp`           | Shows wielded weapon                        |
| `SaveLoad.h/cpp`    | Format version 3                            |
