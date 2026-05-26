# Animated Character (Inheritance) — Chapter 19

In-book project for Chapter 19. The class architecture is built on top of
the Chapter 17 demo (`Animator`, `HUD`) — those files are reused without
change. New for this chapter: a base `Entity` class, plus `Player` and
`Enemy` derived from it, all driven through a polymorphic
`std::vector<std::unique_ptr<Entity>>` in `main.cpp`.

Read order:
1. `Animator.h` (review — unchanged from Ch 17)
2. `Entity.h` / `Entity.cpp` — the base class
3. `Player.h` / `Player.cpp` — minimal subclass
4. `Enemy.h` / `Enemy.cpp` — overrides update + render
5. `main.cpp` — polymorphic loop

A richer asset-driven version of the same architecture lives in the
sibling **Animated character 2 (Inheritance)** folder. Core SDL 3 only
for this version — no add-on libraries required.
