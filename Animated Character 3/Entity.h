#pragma once
#include <SDL3/SDL.h>
#include "IUpdatable.h"
#include "IDrawable.h"
#include "Animator.h"

/* ============================================================================
 *  Entity  -  TUTORIAL: A class that IMPLEMENTS interfaces
 * ============================================================================
 *
 *  Compared to the previous project, Entity has only one structural
 *  change: it now inherits from TWO interfaces.
 *
 *      class Entity : public IUpdatable, public IDrawable { ... };
 *
 *  Read this as: "Entity promises to fulfil the IUpdatable contract
 *  AND the IDrawable contract." Inheriting from multiple interfaces is
 *  fine - they have no state to clash. (See IUpdatable.h.)
 *
 *  Entity still has data of its own (animator_, x_, y_, etc.) and still
 *  acts as a base class for Player and Enemy. So Entity wears two hats:
 *
 *      - Towards Player and Enemy: Entity is an ABSTRACT-BASE-CLASS-LIKE
 *        thing that provides shared state and a default behaviour.
 *
 *      - Towards the rest of the program: Entity is "something that
 *        implements IUpdatable and IDrawable." Code that only knows
 *        about the interfaces can still use Entity-derived objects.
 *
 *  Notice we no longer need to write `virtual ~Entity()` ourselves.
 *  Both interfaces already declare `virtual ~ = default`, and that
 *  virtuality propagates: ~Entity is virtual automatically. Less code,
 *  same correctness.
 *
 *
 *  HOW PURE-VIRTUAL METHODS GET FULFILLED
 *
 *  IUpdatable declares  virtual void update(float) = 0;
 *  IDrawable  declares  virtual void render(SDL_Renderer*) const = 0;
 *
 *  Entity provides bodies for BOTH (see Entity.cpp). That fulfils the
 *  contracts, so Entity is now a CONCRETE class - you can instantiate
 *  it. (Player and Enemy will go further and override these methods
 *  with their own behaviour, but Entity is no longer abstract.)
 *
 *  The methods are still `virtual` after Entity provides bodies, so
 *  derived classes can keep on overriding them. That's how Enemy
 *  manages to add movement on top of Entity's defaults.
 *
 *
 *  WHY USE BOTH AN INHERITANCE HIERARCHY AND INTERFACES?
 *
 *  Inheritance handles "Player and Enemy share state and code."
 *  Interfaces handle "Entity and HUD share a contract with the rest
 *  of the program even though they share nothing else."
 *
 *  These two design tools solve different problems. A real codebase
 *  uses both at the same time, exactly like we do here.
 * ========================================================================== */

class Entity : public IUpdatable, public IDrawable {
public:
    Entity(SDL_Texture* sheet,
           int   frameCount,
           float frameDuration,
           float x,
           float y,
           float drawWidth,
           float drawHeight);

    // No `virtual ~Entity()` needed - the interfaces already provide
    // a virtual destructor, so ours is virtual by inheritance. Keeping
    // less code reduces the chance of the rule being misremembered.

    Entity(const Entity&)            = delete;
    Entity& operator=(const Entity&) = delete;

    // These two methods OVERRIDE the pure-virtual declarations in
    // IUpdatable and IDrawable respectively. The compiler verifies
    // (via `override`) that they really do override something.
    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

protected:
    Animator animator_;
    float    x_;
    float    y_;
    float    drawWidth_;
    float    drawHeight_;
};
