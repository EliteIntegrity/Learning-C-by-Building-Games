#pragma once
#include "Entity.h"

/* ============================================================================
 *  Enemy  -  TUTORIAL: Overriding virtual functions
 * ============================================================================
 *
 *  Enemy is also a Kind-Of Entity, but it differs from Player in three ways:
 *
 *      1. It MOVES across the screen (and wraps around when it leaves).
 *      2. It is drawn FLIPPED HORIZONTALLY (running the other way).
 *      3. It is drawn TINTED (each enemy gets its own colour).
 *
 *  To get those differences we OVERRIDE two of Entity's virtual methods:
 *
 *      void update(float dt) override;      // adds movement
 *      void render(SDL_Renderer*) const override;   // applies flip
 *
 *  WHAT `override` DOES
 *
 *  Strictly, `override` is optional - you can override a virtual method
 *  without writing the keyword. But putting it in is a SAFETY NET. If
 *  you misspell the name or get the parameter list slightly wrong, the
 *  compiler will refuse to build instead of silently creating a brand
 *  new unrelated method that never gets called. Always write `override`.
 *
 *  HOW WE TINT
 *
 *  Each Enemy owns its own copy of the sprite texture (because each owns
 *  its own Animator, see the project notes). In the constructor we call
 *  SDL_SetTextureColorMod ONCE on that personal texture. The colour
 *  multiply is sticky - every subsequent draw will use it - and it
 *  never affects other enemies or the player.
 *
 *  HOW WE FLIP
 *
 *  SDL_RenderTexture has no flip parameter, so we use its sibling
 *  SDL_RenderTextureRotated, passing 0 degrees of rotation and
 *  SDL_FLIP_HORIZONTAL. (See Enemy.cpp.)
 *
 *  HOW WE MOVE
 *
 *  Enemy::update first calls Entity::update so the animation still
 *  advances - we don't want to lose the inherited behaviour, just add
 *  to it. Then we subtract from x_ to scroll left, and wrap when off
 *  screen. This pattern - "do what the base class does, plus extra" -
 *  is extremely common, and the syntax for it is:
 *
 *      Entity::update(dt);     // call base version explicitly
 *      x_ -= speed_ * dt;      // then add our own logic
 *
 *  Without the `Entity::` prefix you'd be calling Enemy::update again
 *  and recursing forever, so don't forget it.
 * ========================================================================== */

class Enemy : public Entity {
public:
    Enemy(SDL_Texture* sheet,
          int   frameCount,
          float frameDuration,
          float startX,
          float startY,
          float drawWidth,
          float drawHeight,
          float speed,             // pixels per second, leftward
          float screenWidth,       // for wrap-around bookkeeping
          SDL_Color tint);         // permanent colour multiply

    // We override two of Entity's virtual methods. The `override`
    // keyword tells the compiler to verify these really do override
    // a base method - if Entity ever changes its signatures, we'll
    // get a clear compile error here instead of silent breakage.
    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

private:
    float speed_;
    float screenWidth_;
};
