// ============================================================
//  Bouncing Ball  -  Chapter 3 project
// ============================================================
//
//  A single ball (drawn as a square) starts in the center of
//  an 800x600 window, drifts at a constant velocity, and
//  bounces off all four walls forever.
//
//  This is the project from Chapter 3 of "Learning C++ by
//  Building Games". It puts the variable types covered in
//  Chapter 2 to work visually.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

const int   WINDOW_W    = 800;
const int   WINDOW_H    = 600;
const float BALL_RADIUS = 20.0f;

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Bouncing Ball",
        WINDOW_W, WINDOW_H,
        0
    );
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

    // The ball's state: position and velocity.
    float ballX    = 400.0f;
    float ballY    = 300.0f;
    float ballVelX = 300.0f;     // pixels/sec, positive = right
    float ballVelY = 250.0f;     // pixels/sec, positive = down

    Uint64 lastTime = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        // Delta time in seconds.
        Uint64 now       = SDL_GetTicks();
        float  deltaTime = (now - lastTime) / 1000.0f;
        lastTime         = now;

        // Move the ball.
        ballX += ballVelX * deltaTime;
        ballY += ballVelY * deltaTime;

        // Bounce off the four walls.
        if (ballX - BALL_RADIUS < 0)
        {
            ballX    = BALL_RADIUS;
            ballVelX = -ballVelX;
        }
        if (ballX + BALL_RADIUS > WINDOW_W)
        {
            ballX    = WINDOW_W - BALL_RADIUS;
            ballVelX = -ballVelX;
        }
        if (ballY - BALL_RADIUS < 0)
        {
            ballY    = BALL_RADIUS;
            ballVelY = -ballVelY;
        }
        if (ballY + BALL_RADIUS > WINDOW_H)
        {
            ballY    = WINDOW_H - BALL_RADIUS;
            ballVelY = -ballVelY;
        }

        // Render.
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);     // black background
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);   // red ball
        SDL_FRect ballRect = {
            ballX - BALL_RADIUS,
            ballY - BALL_RADIUS,
            BALL_RADIUS * 2.0f,
            BALL_RADIUS * 2.0f
        };
        SDL_RenderFillRect(renderer, &ballRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
