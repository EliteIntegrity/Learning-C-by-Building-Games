#pragma once
#include "Entity.h"

/* ============================================================================
 *  Enemy  -  TUTORIAL: Also unchanged from the previous project
 * ============================================================================
 *
 *  Like Player, Enemy doesn't need to know about the interfaces.
 *  It still inherits from Entity, still overrides update() and
 *  render() with `override`, still applies tint and flip.
 *
 *  Where IS the interface visible? Only in main.cpp, where the
 *  POLYMORPHIC LISTS are typed as IUpdatable* and IDrawable*. The
 *  rest of the program continues to work in terms of concrete classes.
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
          float speed,
          float screenWidth,
          SDL_Color tint);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) const override;

private:
    float speed_;
    float screenWidth_;
};
