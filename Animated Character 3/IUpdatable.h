#pragma once

/* ============================================================================
 *  IUpdatable  -  TUTORIAL: What is an interface?
 * ============================================================================
 *
 *  C++ has no `interface` keyword. An interface in C++ is just a class
 *  with these properties:
 *
 *      1. ALL its functions are PURE VIRTUAL  ( = 0 )
 *      2. It has NO data members
 *      3. It has a VIRTUAL DESTRUCTOR (so deleting through an interface
 *         pointer cleans up the derived type properly)
 *
 *  That's it. There is no special syntax - just a class shaped like a
 *  contract. Convention is to prefix the name with `I` so readers know
 *  at a glance ("IUpdatable", "IDrawable") that they're looking at a
 *  contract, not a real object you can instantiate.
 *
 *
 *  WHY HAVE INTERFACES SEPARATE FROM A BASE CLASS?
 *
 *  In the previous project (Animated character 2) we had `Entity` as
 *  a base class with state (animator, position, size) AND virtual
 *  methods (update, render). Player and Enemy inherited from Entity
 *  and got both the state AND the methods.
 *
 *  But what if a class has nothing to do with an Entity, yet still
 *  wants to participate in the same update loop? In our project that
 *  is HUD - it has its own font and FPS counter, no animator, no
 *  position. It would be wrong to make HUD inherit from Entity just
 *  to get update() called on it.
 *
 *  An interface is the answer. We say:
 *
 *      "Anything that knows how to be updated may IMPLEMENT IUpdatable."
 *
 *  Now Entity implements IUpdatable, AND HUD implements IUpdatable,
 *  even though Entity and HUD share nothing else. The main loop can
 *  iterate over a list of IUpdatable* and tick all of them with the
 *  same line of code.
 *
 *
 *  INTERFACE vs ABSTRACT BASE CLASS
 *
 *      Abstract base class:  "is-a" relationship, often shares state.
 *                            Used when several classes ARE KINDS OF
 *                            something (Player IS-A Entity).
 *
 *      Interface:            a CONTRACT. No state. Often implemented
 *                            by completely unrelated classes that
 *                            happen to support the same operation.
 *
 *  Use Entity when classes share STATE and BEHAVIOUR.
 *  Use an interface when classes only need to share a CONTRACT.
 *
 *
 *  MULTIPLE INHERITANCE - SAFE WHEN IT'S INTERFACES ONLY
 *
 *  C++ allows a class to inherit from many classes at once. With
 *  stateful base classes this is dangerous (the dreaded "diamond
 *  problem"). But inheriting from MANY INTERFACES is fine and common -
 *  Entity will inherit from BOTH IUpdatable AND IDrawable in this
 *  project. Each interface contributes only a contract (no data),
 *  so there's nothing to clash.
 *
 *
 *  WHY THIS FILE HAS NO .cpp
 *
 *  An interface has no implementation - every method is = 0. There's
 *  literally nothing to compile in a .cpp file. Pure header.
 * ========================================================================== */

class IUpdatable {
public:
    // Virtual destructor: critical. If anyone deletes a derived object
    // through an IUpdatable*, this is what makes the derived destructor
    // actually run. Forget this and you get silent leaks.
    virtual ~IUpdatable() = default;

    // The contract: implementers MUST provide an update method.
    // The "= 0" makes it PURE VIRTUAL: no body here, every concrete
    // subclass MUST supply one or the compiler refuses to instantiate.
    virtual void update(float dt) = 0;
};
