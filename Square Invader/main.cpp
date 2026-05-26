// ============================================================
//  Square Invader  -  Chapter 5 project
// ============================================================
//
//  A green player at the bottom of the screen slides left/right
//  with A and D. Space fires a single yellow bullet upward.
//  A red invader marches across the top, drops down a row at
//  each wall, and slowly closes on the player.
//
//  Shoot the invader: score +1, invader resets to the top.
//  Let the invader reach your row: lose a life, invader resets.
//  Run out of lives: game over (background turns dark red,
//  player dims). Press Escape or close the window to quit.
//
//  This is the project from Chapter 5 of "Learning C++ by
//  Building Games". It puts the flow-control features from
//  Chapter 4 to work alongside the variables from Chapter 2.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   SCREEN_W       = 800;
const int   SCREEN_H       = 600;
const int   PLAYER_SIZE    = 40;
const int   BULLET_SIZE    = 10;
const int   INVADER_SIZE   = 40;
const float PLAYER_SPEED   = 5.0f;   // pixels per frame
const float BULLET_SPEED   = 8.0f;
const float INVADER_SPEED  = 2.0f;
const int   STARTING_LIVES = 1;

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Square Invader", SCREEN_W, SCREEN_H, 0);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Player position.
    float playerX = (SCREEN_W - PLAYER_SIZE) / 2.0f;
    float playerY = SCREEN_H - PLAYER_SIZE - 10.0f;

    // Bullet (only one in flight at a time).
    float bulletX      = 0.0f;
    float bulletY      = 0.0f;
    bool  bulletActive = false;

    // Invader.
    float invaderX      = 0.0f;
    float invaderY      = 10.0f;
    float invaderSpeedX = INVADER_SPEED;   // positive = moving right

    // Game state.
    int  score    = 0;
    int  lives    = STARTING_LIVES;
    bool gameOver = false;

    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_KEY_DOWN:
                    if (event.key.scancode == SDL_SCANCODE_ESCAPE)
                    {
                        running = false;
                    }
                    else if (event.key.scancode == SDL_SCANCODE_SPACE
                             && !bulletActive
                             && !gameOver)
                    {
                        // Spawn a bullet centered on top of the player.
                        bulletX      = playerX + (PLAYER_SIZE - BULLET_SIZE) / 2.0f;
                        bulletY      = playerY;
                        bulletActive = true;
                    }
                    break;
            }
        }

        if (!gameOver)
        {
            const bool* keys = SDL_GetKeyboardState(nullptr);

            if (keys[SDL_SCANCODE_A]) playerX -= PLAYER_SPEED;
            if (keys[SDL_SCANCODE_D]) playerX += PLAYER_SPEED;

            if (playerX < 0.0f)                   playerX = 0.0f;
            if (playerX > SCREEN_W - PLAYER_SIZE) playerX = SCREEN_W - PLAYER_SIZE;
        }

        if (bulletActive && !gameOver)
        {
            bulletY -= BULLET_SPEED;   // negative Y = moving up

            if (bulletY + BULLET_SIZE < 0.0f)
                bulletActive = false;

            // Axis-aligned bounding box overlap check.
            bool overlapX = bulletX < invaderX + INVADER_SIZE &&
                            bulletX + BULLET_SIZE > invaderX;
            bool overlapY = bulletY < invaderY + INVADER_SIZE &&
                            bulletY + BULLET_SIZE > invaderY;

            if (overlapX && overlapY)
            {
                // Hit! Score, reset bullet and invader.
                score        += 1;
                bulletActive = false;

                invaderX      = 0.0f;
                invaderY      = 10.0f;
                invaderSpeedX = INVADER_SPEED;
            }
        }

        if (!gameOver)
        {
            invaderX += invaderSpeedX;

            // Right wall: drop down and head left.
            if (invaderX + INVADER_SIZE >= SCREEN_W)
            {
                invaderX      = SCREEN_W - INVADER_SIZE;
                invaderY     += INVADER_SIZE;
                invaderSpeedX = -INVADER_SPEED;
            }
            // Left wall: drop down and head right.
            else if (invaderX <= 0.0f)
            {
                invaderX      = 0.0f;
                invaderY     += INVADER_SIZE;
                invaderSpeedX = INVADER_SPEED;
            }

            // Invader reached the player's row?
            if (invaderY + INVADER_SIZE >= playerY)
            {
                lives -= 1;

                invaderX      = 0.0f;
                invaderY      = 10.0f;
                invaderSpeedX = INVADER_SPEED;

                gameOver = (lives <= 0);
            }
        }

        // -- Render --
        if (gameOver)
            SDL_SetRenderDrawColor(renderer, 60, 0, 0, 255);    // game-over red
        else
            SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);  // normal dark
        SDL_RenderClear(renderer);

        // Player (green; dimmer when game over).
        int playerGreen = gameOver ? 80 : 200;
        SDL_SetRenderDrawColor(renderer, 0, playerGreen, 0, 255);
        SDL_FRect playerRect = { playerX, playerY, (float)PLAYER_SIZE, (float)PLAYER_SIZE };
        SDL_RenderFillRect(renderer, &playerRect);

        if (bulletActive)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_FRect bulletRect = { bulletX, bulletY, (float)BULLET_SIZE, (float)BULLET_SIZE };
            SDL_RenderFillRect(renderer, &bulletRect);
        }

        SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
        SDL_FRect invaderRect = { invaderX, invaderY, (float)INVADER_SIZE, (float)INVADER_SIZE };
        SDL_RenderFillRect(renderer, &invaderRect);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);   // ~60 FPS
    }

    SDL_Log("Game over! Final score: %d  Lives left: %d", score, lives);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
