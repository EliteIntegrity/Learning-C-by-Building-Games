# Loot Grid — Chapter 15

In-book project for Chapter 15 of *Learning C++ by Building Games*. Source
in `main.cpp` matches the chapter's complete listing.

A small grid-based collecting game built around several flavors of
`std::map` / `std::unordered_map` and `std::pair`. Strictly procedural —
the OOP refactor lives in later chapters (Animated Character family).

## Controls

- **WASD** — move one cell
- **TAB** — log current inventory and remaining items
- **R** — new random world
- **Esc** — quit

## To build

Empty C++ project per the Chapter 1 setup steps, then drop in `main.cpp`.
Core SDL 3 only — no add-on libraries.
