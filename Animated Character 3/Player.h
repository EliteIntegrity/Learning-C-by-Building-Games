#pragma once
#include "Entity.h"

/* ============================================================================
 *  Player  -  TUTORIAL: Notice what does NOT change
 * ============================================================================
 *
 *  Open this file side-by-side with Player.h from the previous project.
 *  They are IDENTICAL. Player has no idea that Entity has gained two
 *  interface parents. It still inherits from Entity exactly as before.
 *
 *  This is encapsulation paying you back. Adding interfaces to Entity
 *  was an additive change - we only declared MORE about what Entity
 *  promises to do. Existing subclasses keep working without touching
 *  a single line.
 *
 *  THE BIG IDEA: well-chosen abstractions let you grow a program
 *  upwards (more contracts, more uses) without disturbing the
 *  existing layers below. This is the open/closed principle in
 *  practice: open for extension, closed for modification.
 * ========================================================================== */

class Player : public Entity {
public:
    Player(SDL_Texture* sheet,
           int   frameCount,
           float frameDuration,
           float screenWidth,
           float screenHeight);
};
