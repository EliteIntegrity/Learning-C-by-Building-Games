#include <SDL3/SDL.h>
#include <cstdlib>
#include <cstring>

// ============================================================
// CONSTANTS
// ============================================================
static const int   SCREEN_W       = 800;
static const int   SCREEN_H       = 600;

static const int   PLAYER_W       = 40;
static const int   PLAYER_H       = 20;
static const float PLAYER_SPEED   = 300.0f;   // px/sec

static const int   BULLET_W       = 6;
static const int   BULLET_H       = 14;
static const float P_BULLET_SPEED = 500.0f;   // player bullet px/sec (upward)
static const float E_BULLET_SPEED = 220.0f;   // enemy bullet px/sec (downward)

static const int   INV_COLS       = 11;
static const int   INV_ROWS       = 5;
static const int   INV_W          = 32;
static const int   INV_H          = 20;
static const int   INV_PAD_X      = 16;
static const int   INV_PAD_Y      = 12;
static const float INV_SPEED_BASE = 60.0f;    // px/sec, grows as invaders die
static const float INV_DROP       = 24.0f;    // px to drop on edge bounce
static const float INV_SHOOT_INT  = 1.2f;     // seconds between enemy shots

static const int   BARRIER_COUNT  = 4;
static const int   BARRIER_W      = 60;
static const int   BARRIER_H      = 30;
static const int   BARRIER_HP     = 6;

static const int   PLAYER_LIVES   = 3;
static const int   LIFE_W         = 20;
static const int   LIFE_H         = 10;

// ============================================================
// STRUCTS
// ============================================================
struct Invader {
    float x, y;
    bool  alive;
    int   row;   // 0=top, used for colour
};

struct Bullet {
    float x, y;
    bool  active;
};

struct Barrier {
    float x, y;
    int   hp;
};

// ============================================================
// HELPERS
// ============================================================
static bool rectsOverlap(float ax, float ay, float aw, float ah,
                         float bx, float by, float bw, float bh)
{
    return ax < bx + bw && ax + aw > bx &&
           ay < by + bh && ay + ah > by;
}

static void renderRect(SDL_Renderer* r, float x, float y, float w, float h)
{
    SDL_FRect rc = { x, y, w, h };
    SDL_RenderFillRect(r, &rc);
}

// ============================================================
// MAIN
// ============================================================
int mainB(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window*   window   = SDL_CreateWindow("Square Invader", SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!window || !renderer) {
        SDL_Log("Window/Renderer failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // --------------------------------------------------------
    // STATE
    // --------------------------------------------------------
    enum class State { PLAYING, DEAD_PAUSE, GAME_OVER, WIN };
    State state = State::PLAYING;

    // Player
    float playerX    = (SCREEN_W - PLAYER_W) / 2.0f;
    float playerY    = SCREEN_H - 60.0f;
    int   lives      = PLAYER_LIVES;
    float respawnTimer = 0.0f;

    // Player bullet (only one at a time)
    Bullet pBullet = {};

    // Enemy bullets (up to 3 at once)
    static const int MAX_E_BULLETS = 3;
    Bullet eBullets[MAX_E_BULLETS] = {};

    // Invaders
    Invader invaders[INV_ROWS][INV_COLS];
    int totalAlive = INV_ROWS * INV_COLS;

    // Grid origin so the whole grid shifts together
    float gridX = 40.0f;
    float gridY = 60.0f;
    float gridSpeedX = INV_SPEED_BASE;
    float shootTimer = INV_SHOOT_INT;

    for (int row = 0; row < INV_ROWS; ++row)
        for (int col = 0; col < INV_COLS; ++col) {
            invaders[row][col].x     = gridX + col * (INV_W + INV_PAD_X);
            invaders[row][col].y     = gridY + row * (INV_H + INV_PAD_Y);
            invaders[row][col].alive = true;
            invaders[row][col].row   = row;
        }

    // Barriers
    Barrier barriers[BARRIER_COUNT];
    {
        float gap = SCREEN_W / (float)(BARRIER_COUNT + 1);
        for (int i = 0; i < BARRIER_COUNT; ++i) {
            barriers[i].x  = gap * (i + 1) - BARRIER_W / 2.0f;
            barriers[i].y  = SCREEN_H - 130.0f;
            barriers[i].hp = BARRIER_HP;
        }
    }

    // Score (counted as invaders killed * 10)
    int score = 0;

    // Timing
    Uint64 prevTick = SDL_GetTicks();

    // --------------------------------------------------------
    // GAME LOOP
    // --------------------------------------------------------
    bool running = true;
    while (running) {

        Uint64 now = SDL_GetTicks();
        float  dt  = (now - prevTick) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f;   // clamp if window dragged etc.
        prevTick = now;

        // ----------------------------------------------------
        // EVENTS
        // ----------------------------------------------------
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == SDL_SCANCODE_ESCAPE)
                    running = false;

                // Fire
                if (event.key.scancode == SDL_SCANCODE_SPACE &&
                    state == State::PLAYING && !pBullet.active)
                {
                    pBullet.x      = playerX + (PLAYER_W - BULLET_W) / 2.0f;
                    pBullet.y      = playerY - BULLET_H;
                    pBullet.active = true;
                }

                // Restart from GAME_OVER or WIN
                if ((state == State::GAME_OVER || state == State::WIN) &&
                    event.key.scancode == SDL_SCANCODE_RETURN)
                {
                    // Reset everything
                    playerX = (SCREEN_W - PLAYER_W) / 2.0f;
                    lives   = PLAYER_LIVES;
                    pBullet = {};
                    for (auto& b : eBullets) b = {};
                    score        = 0;
                    totalAlive   = INV_ROWS * INV_COLS;
                    gridX        = 40.0f;
                    gridY        = 60.0f;
                    gridSpeedX   = INV_SPEED_BASE;
                    shootTimer   = INV_SHOOT_INT;
                    for (int row = 0; row < INV_ROWS; ++row)
                        for (int col = 0; col < INV_COLS; ++col) {
                            invaders[row][col].x     = gridX + col * (INV_W + INV_PAD_X);
                            invaders[row][col].y     = gridY + row * (INV_H + INV_PAD_Y);
                            invaders[row][col].alive = true;
                        }
                    for (int i = 0; i < BARRIER_COUNT; ++i)
                        barriers[i].hp = BARRIER_HP;
                    state = State::PLAYING;
                }
            }
        }

        // ----------------------------------------------------
        // UPDATE
        // ----------------------------------------------------
        if (state == State::PLAYING) {

            const bool* keys = SDL_GetKeyboardState(nullptr);
            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
                playerX -= PLAYER_SPEED * dt;
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
                playerX += PLAYER_SPEED * dt;
            playerX = SDL_clamp(playerX, 0.0f, (float)(SCREEN_W - PLAYER_W));

            // Player bullet
            if (pBullet.active) {
                pBullet.y -= P_BULLET_SPEED * dt;
                if (pBullet.y + BULLET_H < 0) pBullet.active = false;

                // vs invaders
                for (int row = 0; row < INV_ROWS && pBullet.active; ++row)
                    for (int col = 0; col < INV_COLS && pBullet.active; ++col) {
                        Invader& inv = invaders[row][col];
                        if (!inv.alive) continue;
                        if (rectsOverlap(pBullet.x, pBullet.y, BULLET_W, BULLET_H,
                                         inv.x, inv.y, INV_W, INV_H))
                        {
                            inv.alive      = false;
                            pBullet.active = false;
                            --totalAlive;
                            score += 10 + (INV_ROWS - 1 - row) * 5;
                            // Speed up remaining invaders
                            gridSpeedX = (gridSpeedX > 0 ? 1 : -1) *
                                (INV_SPEED_BASE + (INV_ROWS * INV_COLS - totalAlive) * 4.0f);
                        }
                    }

                // vs barriers
                for (int i = 0; i < BARRIER_COUNT && pBullet.active; ++i) {
                    if (barriers[i].hp <= 0) continue;
                    if (rectsOverlap(pBullet.x, pBullet.y, BULLET_W, BULLET_H,
                                     barriers[i].x, barriers[i].y, BARRIER_W, BARRIER_H))
                    {
                        --barriers[i].hp;
                        pBullet.active = false;
                    }
                }
            }

            // Move invader grid
            gridX += gridSpeedX * dt;

            // Find grid extents
            float minX = 9999, maxX = -9999;
            for (int row = 0; row < INV_ROWS; ++row)
                for (int col = 0; col < INV_COLS; ++col) {
                    if (!invaders[row][col].alive) continue;
                    float lx = gridX + col * (INV_W + INV_PAD_X);
                    float rx = lx + INV_W;
                    if (lx < minX) minX = lx;
                    if (rx > maxX) maxX = rx;
                }

            if (gridSpeedX > 0 && maxX >= SCREEN_W) {
                gridX      -= (maxX - SCREEN_W);
                gridY      += INV_DROP;
                gridSpeedX  = -(fabsf(gridSpeedX));
            }
            if (gridSpeedX < 0 && minX <= 0) {
                gridX      -= minX;          // push back to edge
                gridY      += INV_DROP;
                gridSpeedX  = fabsf(gridSpeedX);
            }

            // Update each invader position from grid
            for (int row = 0; row < INV_ROWS; ++row)
                for (int col = 0; col < INV_COLS; ++col) {
                    invaders[row][col].x = gridX + col * (INV_W + INV_PAD_X);
                    invaders[row][col].y = gridY + row * (INV_H + INV_PAD_Y);
                }

            // Invaders reached player line -> game over
            for (int row = 0; row < INV_ROWS; ++row)
                for (int col = 0; col < INV_COLS; ++col)
                    if (invaders[row][col].alive &&
                        invaders[row][col].y + INV_H >= playerY)
                        state = State::GAME_OVER;

            // Enemy shooting
            shootTimer -= dt;
            if (shootTimer <= 0.0f) {
                shootTimer = INV_SHOOT_INT * (0.5f + (float)totalAlive / (INV_ROWS * INV_COLS));

                // Pick a random alive invader in the bottom-most occupied column
                // Build list of bottom-most invaders per column
                int shooterRow[INV_COLS], shooterCol[INV_COLS];
                int shooterCount = 0;
                for (int col = 0; col < INV_COLS; ++col) {
                    int bottomRow = -1;
                    for (int row = INV_ROWS - 1; row >= 0; --row)
                        if (invaders[row][col].alive) { bottomRow = row; break; }
                    if (bottomRow >= 0) {
                        shooterRow[shooterCount]   = bottomRow;
                        shooterCol[shooterCount++] = col;
                    }
                }
                if (shooterCount > 0) {
                    int idx = rand() % shooterCount;
                    Invader& shooter = invaders[shooterRow[idx]][shooterCol[idx]];
                    for (int i = 0; i < MAX_E_BULLETS; ++i) {
                        if (!eBullets[i].active) {
                            eBullets[i].x      = shooter.x + (INV_W - BULLET_W) / 2.0f;
                            eBullets[i].y      = shooter.y + INV_H;
                            eBullets[i].active = true;
                            break;
                        }
                    }
                }
            }

            // Move enemy bullets
            for (int i = 0; i < MAX_E_BULLETS; ++i) {
                if (!eBullets[i].active) continue;
                eBullets[i].y += E_BULLET_SPEED * dt;
                if (eBullets[i].y > SCREEN_H) { eBullets[i].active = false; continue; }

                // vs player
                if (rectsOverlap(eBullets[i].x, eBullets[i].y, BULLET_W, BULLET_H,
                                  playerX, playerY, PLAYER_W, PLAYER_H))
                {
                    eBullets[i].active = false;
                    --lives;
                    pBullet.active = false;
                    if (lives <= 0) { state = State::GAME_OVER; }
                    else            { state = State::DEAD_PAUSE; respawnTimer = 1.5f; }
                }

                // vs barriers
                for (int b = 0; b < BARRIER_COUNT; ++b) {
                    if (barriers[b].hp <= 0) continue;
                    if (rectsOverlap(eBullets[i].x, eBullets[i].y, BULLET_W, BULLET_H,
                                     barriers[b].x, barriers[b].y, BARRIER_W, BARRIER_H))
                    {
                        --barriers[b].hp;
                        eBullets[i].active = false;
                        break;
                    }
                }
            }

            if (totalAlive == 0) state = State::WIN;

        } else if (state == State::DEAD_PAUSE) {
            respawnTimer -= dt;
            if (respawnTimer <= 0.0f) {
                playerX = (SCREEN_W - PLAYER_W) / 2.0f;
                state   = State::PLAYING;
            }
        }

        // ----------------------------------------------------
        // RENDER
        // ----------------------------------------------------
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Invaders - colour by row
        static const SDL_Color invColors[INV_ROWS] = {
            {255, 80,  80,  255},   // row 0 top    - red
            {255, 160, 50,  255},   // row 1        - orange
            {255, 255, 50,  255},   // row 2 middle - yellow
            {50,  255, 100, 255},   // row 3        - green
            {80,  160, 255, 255},   // row 4 bottom - blue
        };
        for (int row = 0; row < INV_ROWS; ++row) {
            SDL_SetRenderDrawColor(renderer,
                invColors[row].r, invColors[row].g, invColors[row].b, 255);
            for (int col = 0; col < INV_COLS; ++col) {
                if (!invaders[row][col].alive) continue;
                renderRect(renderer,
                    invaders[row][col].x, invaders[row][col].y, INV_W, INV_H);
            }
        }

        // Barriers (fade to dark as hp drops)
        for (int i = 0; i < BARRIER_COUNT; ++i) {
            if (barriers[i].hp <= 0) continue;
            int bright = 60 + (barriers[i].hp * 195) / BARRIER_HP;
            SDL_SetRenderDrawColor(renderer, 0, (Uint8)bright, 0, 255);
            renderRect(renderer, barriers[i].x, barriers[i].y, BARRIER_W, BARRIER_H);
        }

        // Player (white, hidden during death pause flash)
        bool showPlayer = (state == State::PLAYING) ||
                          (state == State::DEAD_PAUSE &&
                           (int)(respawnTimer * 6) % 2 == 0);
        if (showPlayer) {
            SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
            renderRect(renderer, playerX, playerY, PLAYER_W, PLAYER_H);
        }

        // Player bullet (cyan)
        if (pBullet.active) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            renderRect(renderer, pBullet.x, pBullet.y, BULLET_W, BULLET_H);
        }

        // Enemy bullets (orange-red)
        SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
        for (int i = 0; i < MAX_E_BULLETS; ++i)
            if (eBullets[i].active)
                renderRect(renderer, eBullets[i].x, eBullets[i].y, BULLET_W, BULLET_H);

        // HUD - lives as small squares bottom-left
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        for (int i = 0; i < lives; ++i)
            renderRect(renderer, 10.0f + i * (LIFE_W + 6), SCREEN_H - 20.0f,
                       LIFE_W, LIFE_H);

        // Score bar - one pixel-wide column per 10 pts, top-right area
        SDL_SetRenderDrawColor(renderer, 180, 180, 60, 255);
        float scoreBarW = (float)(score / 10) * 3.0f;
        if (scoreBarW > 200) scoreBarW = 200;
        renderRect(renderer, SCREEN_W - scoreBarW - 10.0f, 10.0f, scoreBarW, 10.0f);

        // GAME OVER screen - big red blocks spelling out "X X"
        if (state == State::GAME_OVER) {
            SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
            // Four diagonal bars forming an X (left)
            for (int i = 0; i < 8; ++i) {
                renderRect(renderer, 280.0f + i*14, 260.0f + i*14, 14, 14);
                renderRect(renderer, 280.0f + i*14, 358.0f - i*14, 14, 14);
            }
            // ENTER hint - small green row of blocks
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            for (int i = 0; i < 10; ++i)
                renderRect(renderer, 330.0f + i*14, 430.0f, 10, 10);
        }

        // WIN screen - yellow blocks
        if (state == State::WIN) {
            SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
            for (int i = 0; i < 12; ++i)
                renderRect(renderer, 240.0f + i*26, 280.0f, 20, 20);
            for (int i = 0; i < 12; ++i)
                renderRect(renderer, 240.0f + i*26, 310.0f, 20, 20);
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
            for (int i = 0; i < 10; ++i)
                renderRect(renderer, 300.0f + i*14, 360.0f, 10, 10);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1);   // yield; SDL_RenderPresent already throttles to vsync when available
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
