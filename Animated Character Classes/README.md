# Animated Character (Classes Demo) — Chapter 17

In-book project for Chapter 17 of *Learning C++ by Building Games*. The
chapter walks through each header and source file in detail. Read order:

1. `Animator.h` / `Animator.cpp`
2. `HUD.h` / `HUD.cpp`
3. `Player.h` / `Player.cpp`
4. `main.cpp`

A richer asset-driven version of the same architecture (real sprite
sheet via SDL_image, real TTF font via SDL_ttf) sits in the sibling
**Animated character** folder for readers who want to see what the
same class shape feels like with actual artwork.

## To build

Create an empty C++ project per the Chapter 1 setup. Add all four
`.cpp` files (and the three `.h` files alongside them) to **Source
Files** / **Header Files**. No SDL add-on libraries required — core
SDL 3 only.
