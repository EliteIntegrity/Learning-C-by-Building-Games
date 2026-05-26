#pragma once

// ============================================================
//  IUpdatable  -  Chapter 21
// ============================================================
//
//  A pure abstract interface — what C++ uses where other
//  languages have an `interface` keyword. The contract says
//  "I can be ticked forward by some number of seconds." No
//  data, no state, just the promise.
//
//  Three properties make this a true interface:
//      1. Every method is pure virtual ( = 0 )
//      2. No data members
//      3. Virtual destructor (so deleting through an
//         interface pointer calls the right derived dtor)
//
//  Convention is the `I` prefix so readers see at a glance
//  this is a contract, not a class you'd instantiate.
// ============================================================

class IUpdatable
{
public:
    virtual ~IUpdatable() = default;

    // Pure virtual: implementers MUST provide this method.
    virtual void update(float dt) = 0;
};
