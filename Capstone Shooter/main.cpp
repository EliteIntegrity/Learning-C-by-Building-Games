// ============================================================
//  Act 1 Capstone Shooter  -  Chapter 9 project
// ============================================================
//
//  Stripped-down shooter that pulls together everything from
//  Act 1: variables, flow control, loops, and functions.
//
//  Player: green rectangle, WASD to move.
//  Lasers: pool of 3, Space to fire (rising edge).
//  Aliens: pool of 12, scrolling in from the right.
//  Score, lives, game over, restart with R.
//
//  This is the in-book project from Chapter 9 of "Learning
//  C++ by Building Games". A richer asset-driven version
//  (sprites, fonts, sound) lives in the "Pre OOP Game" folder
//  for readers who want to see what the same architecture
//  feels like with real art.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>
#include <ctime>

// --- Configuration --------------------------------------------------------
const int SCREEN_W = 1024;
const int SCREEN_H = 600;

const float PLAYER_SPEED = 5.0f;
const float LASER_SPEED = 12.0f;
const float ALIEN_MIN_SPD = 2.0f;
const float ALIEN_MAX_SPD = 5.0f;

const int MAX_LASERS = 3;
const int MAX_ALIENS = 12;

const int START_LIVES = 3;
const int POINTS_PER_HIT = 10;

// --- Structs --------------------------------------------------------------
struct Player
{
    float x, y;
    float w, h;
    int   lives;
};

struct Laser
{
    float x, y;
    bool  active;
};

struct Alien
{
    float x, y;
    float w, h;
    float speed;
    bool  active;
};

// --- Helpers --------------------------------------------------------------
float randFloat(float lo, float hi)
{
    float t = (float)rand() / (float)RAND_MAX;
    return lo + t * (hi - lo);
}

bool rectsOverlap(float ax, float ay, float aw, float ah,
    float bx, float by, float bw, float bh)
{
    return ax < bx + bw && ax + aw > bx &&
        ay < by + bh && ay + ah > by;
}

void spawnAlien(Alien& a)
{
    a.w = 48.0f;
    a.h = 48.0f;
    a.x = (float)SCREEN_W + randFloat(0.0f, 200.0f);
    a.y = randFloat(0.0f, (float)SCREEN_H - a.h);
    a.speed = randFloat(ALIEN_MIN_SPD, ALIEN_MAX_SPD);
    a.active = true;
}

void resetGame(Player& p, Laser lasers[], Alien aliens[], int& score)
{
    p.w = 64.0f;
    p.h = 48.0f;
    p.x = 40.0f;
    p.y = (SCREEN_H - p.h) * 0.5f;
    p.lives = START_LIVES;

    for (int i = 0; i < MAX_LASERS; ++i) lasers[i].active = false;
    for (int i = 0; i < MAX_ALIENS; ++i) aliens[i].active = false;

    score = 0;
}

// --- main -----------------------------------------------------------------
int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Act 1 Shooter", SCREEN_W, SCREEN_H, 0);
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

    srand((unsigned)time(nullptr));

    Player player;
    Laser  lasers[MAX_LASERS];
    Alien  aliens[MAX_ALIENS];
    int    score = 0;
    int    highScore = 0;
    bool   gameOver = false;

    resetGame(player, lasers, aliens, score);

    bool spaceWasDown = false;
    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                    running = false;

                if (gameOver && event.key.key == SDLK_R)
                {
                    resetGame(player, lasers, aliens, score);
                    gameOver = false;
                }
            }
        }

        const bool* keys = SDL_GetKeyboardState(nullptr);

        if (!gameOver)
        {
            // -- Player movement --
            if (keys[SDL_SCANCODE_W]) player.y -= PLAYER_SPEED;
            if (keys[SDL_SCANCODE_S]) player.y += PLAYER_SPEED;
            if (keys[SDL_SCANCODE_A]) player.x -= PLAYER_SPEED;
            if (keys[SDL_SCANCODE_D]) player.x += PLAYER_SPEED;

            if (player.x < 0)                       player.x = 0;
            if (player.y < 0)                       player.y = 0;
            if (player.x + player.w > SCREEN_W)     player.x = SCREEN_W - player.w;
            if (player.y + player.h > SCREEN_H)     player.y = SCREEN_H - player.h;

            // -- Fire on rising edge of SPACE --
            bool spaceDown = keys[SDL_SCANCODE_SPACE];
            if (spaceDown && !spaceWasDown)
            {
                for (int i = 0; i < MAX_LASERS; ++i)
                {
                    if (!lasers[i].active)
                    {
                        lasers[i].active = true;
                        lasers[i].x = player.x + player.w;
                        lasers[i].y = player.y + player.h * 0.5f;
                        break;
                    }
                }
            }
            spaceWasDown = spaceDown;

            // -- Update lasers --
            for (int i = 0; i < MAX_LASERS; ++i)
            {
                if (!lasers[i].active) continue;
                lasers[i].x += LASER_SPEED;
                if (lasers[i].x > SCREEN_W) lasers[i].active = false;
            }

            // -- Top up alien pool --
            for (int i = 0; i < MAX_ALIENS; ++i)
            {
                if (!aliens[i].active)
                {
                    spawnAlien(aliens[i]);
                    break;
                }
            }

            // -- Update aliens --
            for (int i = 0; i < MAX_ALIENS; ++i)
            {
                if (!aliens[i].active) continue;
                aliens[i].x -= aliens[i].speed;
                if (aliens[i].x + aliens[i].w < 0)
                    aliens[i].active = false;
            }

            // -- Collisions: lasers vs aliens --
            for (int i = 0; i < MAX_LASERS; ++i)
            {
                if (!lasers[i].active) continue;

                float lw = 16.0f;
                float lh = 2.0f;
                float lx = lasers[i].x - lw;
                float ly = lasers[i].y - 1.0f;

                for (int j = 0; j < MAX_ALIENS; ++j)
                {
                    if (!aliens[j].active) continue;
                    if (rectsOverlap(lx, ly, lw, lh,
                        aliens[j].x, aliens[j].y,
                        aliens[j].w, aliens[j].h))
                    {
                        aliens[j].active = false;
                        lasers[i].active = false;
                        score += POINTS_PER_HIT;
                        break;
                    }
                }
            }

            // -- Collisions: player vs aliens --
            for (int j = 0; j < MAX_ALIENS; ++j)
            {
                if (!aliens[j].active) continue;
                if (rectsOverlap(player.x, player.y, player.w, player.h,
                    aliens[j].x, aliens[j].y,
                    aliens[j].w, aliens[j].h))
                {
                    aliens[j].active = false;
                    player.lives -= 1;

                    if (player.lives <= 0)
                    {
                        gameOver = true;
                        if (score > highScore) highScore = score;
                    }
                }
            }
        }

        // -- Render --
        if (gameOver)
            SDL_SetRenderDrawColor(renderer, 60, 0, 0, 255);
        else
            SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        int green = gameOver ? 80 : 200;
        SDL_SetRenderDrawColor(renderer, 0, green, 0, 255);
        SDL_FRect playerRect = { player.x, player.y, player.w, player.h };
        SDL_RenderFillRect(renderer, &playerRect);

        SDL_SetRenderDrawColor(renderer, 80, 255, 80, 255);
        for (int i = 0; i < MAX_LASERS; ++i)
        {
            if (!lasers[i].active) continue;
            SDL_RenderLine(renderer,
                lasers[i].x - 16.0f, lasers[i].y,
                lasers[i].x, lasers[i].y);
        }

        SDL_SetRenderDrawColor(renderer, 220, 60, 60, 255);
        for (int i = 0; i < MAX_ALIENS; ++i)
        {
            if (!aliens[i].active) continue;
            SDL_FRect r = { aliens[i].x, aliens[i].y, aliens[i].w, aliens[i].h };
            SDL_RenderFillRect(renderer, &r);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_Log("Game ended. Score: %d  High score: %d", score, highScore);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
