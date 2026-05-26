#pragma once
#include <SDL3/SDL.h>
#include "Animator.h"

/* ============================================================================
 *  Entity  -  TUTORIAL: Inheritance and virtual functions
 * ============================================================================
 *
 *  In the previous project all our classes were independent: Player, HUD,
 *  Animator. None of them shared any code. As soon as we want a SECOND
 *  character that behaves slightly differently, copy-pasting Player into
 *  Enemy would give us two near-identical files - any change would have
 *  to be made in both. That's where INHERITANCE earns its keep.
 *
 *  THE BIG IDEA
 *
 *      "Both a Player and an Enemy are KINDS OF Entity."
 *
 *  We promote the shared parts (animator, position, draw size, the basic
 *  update/render logic) into a single base class called Entity. Then
 *  Player and Enemy each inherit from Entity and add or override only
 *  what they need to differ. This is called an "is-a" relationship, and
 *  the syntax for it is:
 *
 *      class Player : public Entity { ... };
 *      class Enemy  : public Entity { ... };
 *
 *
 *  FOUR THINGS TO LEARN FROM THIS HEADER
 *
 *  1. PROTECTED ACCESS
 *     `protected:` is like `private:` to outside code, but DERIVED CLASSES
 *     CAN see it. We put `animator_`, `x_`, `y_`, etc. in protected so
 *     Enemy can move itself by changing its own `x_`. If we'd left them
 *     private, Enemy couldn't touch them.
 *
 *  2. VIRTUAL FUNCTIONS
 *     `virtual void update(float dt);` means "subclasses are allowed to
 *     replace this." When you call entity.update() through a base-class
 *     pointer, C++ looks up which version to call AT RUN TIME based on
 *     the actual object's type. This is the mechanism behind polymorphism.
 *
 *  3. VIRTUAL DESTRUCTOR  (the most common beginner trap!)
 *     If you ever delete a derived object through a base pointer, the
 *     destructor MUST be virtual or only the base destructor runs and
 *     resources owned by the derived part leak. Always:
 *
 *         virtual ~Entity() = default;
 *
 *     Forgetting this is the #1 inheritance bug in real-world C++. Get
 *     in the habit of writing the virtual destructor at the same time
 *     you make a class polymorphic.
 *
 *  4. = default
 *     `= default` tells the compiler "generate the obvious version of
 *     this special member for me." We don't have any cleanup work in
 *     Entity (its Animator member cleans up itself), so the default
 *     destructor is fine - we just need it to be virtual.
 *
 *
 *  WHY ENTITY IS NOT ABSTRACT
 *
 *  We could have written `virtual void update(float) = 0;` to make
 *  Entity an "interface" that cannot be instantiated. That's a useful
 *  step but it's the NEXT lesson. Here, Entity has working default
 *  implementations of update() and render() - Player happily uses them
 *  unchanged, and only Enemy bothers to override.
 * ========================================================================== */

class Entity {
public:
    Entity(SDL_Texture* sheet,
           int   frameCount,
           float frameDuration,
           float x,
           float y,
           float drawWidth,
           float drawHeight);

    // Always make the destructor virtual in a base class meant for
    // inheritance. Without this, deleting an Enemy through an Entity*
    // would skip the Enemy parts of cleanup. = default just asks the
    // compiler to write the body for us.
    virtual ~Entity() = default;

    Entity(const Entity&)            = delete;
    Entity& operator=(const Entity&) = delete;

    // VIRTUAL = subclasses may override. The base provides a sensible
    // default that just advances the animator.
    virtual void update(float dt);

    // VIRTUAL too. Default render draws the current sprite frame
    // un-flipped at (x_, y_) with size (drawWidth_, drawHeight_).
    virtual void render(SDL_Renderer* renderer) const;

protected:
    // PROTECTED: derived classes (Player, Enemy) can read and write these.
    // Keeping them out of `public` means the rest of the program still
    // can't poke at them - encapsulation against the OUTSIDE world only.
    Animator animator_;
    float    x_;
    float    y_;
    float    drawWidth_;
    float    drawHeight_;
};
