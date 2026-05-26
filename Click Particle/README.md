# Click Particle — Chapter 11

In-book project for Chapter 11 of *Learning C++ by Building Games*. Source
in `main.cpp` matches the chapter's complete listing.

Deliberately limited to a single particle on the heap at a time, so the
focus stays on `new`, `delete`, `nullptr`, and the arrow operator.

The Chapter 13 project (in the sibling **Particle Pointers** folder) extends
this same idea using `std::vector<Particle*>` to hold many particles at
once.

## To build

Empty C++ project per the Chapter 1 setup steps, then drop in `main.cpp`.
Core SDL 3 only — no add-on libraries.
