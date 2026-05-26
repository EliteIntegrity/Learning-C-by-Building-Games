# Animated Character (Interfaces) — Chapter 21 / Act 3 Capstone

In-book project for Chapter 21. The Chapter 19 inheritance project,
upgraded with the polymorphism and interface tools from Chapter 20.

Read order:
1. `Animator.h` (review — unchanged from Ch 17)
2. `IUpdatable.h` and `IDrawable.h` — the two interface contracts
3. `Entity.h` / `Entity.cpp` — now implements both interfaces
4. `Player.h` / `Player.cpp` — unchanged from Ch 19
5. `Enemy.h` / `Enemy.cpp` — now uses `override`
6. `HUD.h` / `HUD.cpp` — implements both interfaces independently
7. `main.cpp` — one polymorphic update loop, one polymorphic render loop

A richer asset-driven version (sprite sheet, TTF font) of the same
architecture is in the sibling **Animated Character 3** folder. Core
SDL 3 only for this version — no add-on libraries required.
