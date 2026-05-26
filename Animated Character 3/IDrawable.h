#pragma once
#include <SDL3/SDL.h>

/* ============================================================================
 *  IDrawable  -  TUTORIAL: A second interface, paired with IUpdatable
 * ============================================================================
 *
 *  IDrawable is a sibling of IUpdatable. Read IUpdatable.h first - it
 *  carries the full discussion of what an interface IS and why we use
 *  one. The same rules apply here.
 *
 *  IDrawable is a contract for "I know how to draw myself onto a
 *  renderer." Anything in the program that has a visual representation
 *  - characters, HUD widgets, particle systems, debug overlays -
 *  can implement IDrawable, and the rendering code can paint them all
 *  in one uniform loop.
 *
 *
 *  WHY TWO INTERFACES INSTEAD OF ONE COMBINED ONE?
 *
 *  We could have made a single IGameObject interface with both update
 *  AND render. That works, but it forces every implementer to provide
 *  both methods even when one isn't relevant.
 *
 *  Splitting them is the more flexible design. A class can implement
 *  just one - imagine a static background that needs render() but not
 *  update(), or a sound emitter that needs update() but not render().
 *  This is sometimes called the "interface segregation principle":
 *  prefer many small interfaces over few fat ones, so callers depend
 *  only on what they actually need.
 *
 *  In this project Entity and HUD happen to implement BOTH interfaces.
 *  That's fine and common - the splitting just keeps the option open.
 * ========================================================================== */

class IDrawable {
public:
    virtual ~IDrawable() = default;

    // Pure virtual: every implementer must define this.
    // `const` because rendering should not change the object's state -
    // a HUD that draws itself doesn't update its FPS reading mid-draw.
    virtual void render(SDL_Renderer* renderer) const = 0;
};
