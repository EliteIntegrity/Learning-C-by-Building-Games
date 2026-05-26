#pragma once
#include "Entity.h"

/* ============================================================================
 *  Player  -  TUTORIAL: A minimal subclass
 * ============================================================================
 *
 *  Player is a Kind-Of Entity. Its job is to be a large character
 *  centred on the screen. That's it.
 *
 *  Notice how SHORT this class is. The whole point of inheritance is
 *  that we get update() and render() FOR FREE from Entity - we don't
 *  re-implement them. Player only needs a constructor that figures out
 *  WHERE on the screen to place itself and HOW BIG to be, then hands
 *  those numbers up to the Entity constructor.
 *
 *  THE INHERITANCE SYNTAX
 *
 *      class Player : public Entity { ... };
 *               ^^^^^^^^^^^^^^^^^^^
 *      "Player publicly inherits from Entity."
 *
 *  `public` here means "users of Player can also use Player's Entity
 *  interface" - i.e. a Player IS-A Entity from the outside world's
 *  perspective. This is what lets us put a Player into a container of
 *  Entity pointers (see main.cpp).
 *
 *  HOW THE CONSTRUCTOR DELEGATES UP
 *
 *      Player(...) : Entity(sheet, ..., x, y, w, h) { }
 *                    ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *      "Before doing anything else, build the Entity part of me by
 *       calling the Entity constructor with these arguments."
 *
 *  Construction always proceeds base-first, then derived. The Entity
 *  constructor finishes (animator_, x_, y_ etc. are now valid) before
 *  any Player-specific code runs.
 *
 *  We don't override anything here, so the Entity defaults for update()
 *  and render() are exactly what get called when someone uses a Player.
 *  That's fine - "no override needed" is a perfectly valid design.
 * ========================================================================== */

class Player : public Entity {
public:
    Player(SDL_Texture* sheet,
           int   frameCount,
           float frameDuration,
           float screenWidth,
           float screenHeight);
};
