// ============================================================
//  Vibe Asteroids  -  Chapter 25 vibe-coded project
// ============================================================
//
//  Classic Asteroids. Ship rotates and thrusts; asteroids
//  drift; bullets split asteroids; collisions kill the ship.
//
//  This is the version I ended up with after the multi-round
//  conversation in Chapter 25. Yours will look different.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// --- Configuration --------------------------------------------------------
static constexpr int   WINDOW_W       = 1024;
static constexpr int   WINDOW_H       = 768;

static constexpr float SHIP_RADIUS    = 14.0f;
static constexpr float SHIP_THRUST    = 220.0f;   // accel pixels/sec/sec
static constexpr float SHIP_ROT_SPEED = 4.0f;     // rad/sec
static constexpr float SHIP_DRAG      = 0.6f;     // velocity fraction lost per second

static constexpr float BULLET_SPEED   = 520.0f;
static constexpr float BULLET_LIFE_S  = 0.8f;
static constexpr float FIRE_COOLDOWN_S = 0.18f;

static constexpr int   START_LIVES    = 3;
static constexpr int   START_ASTEROIDS = 4;

// Asteroid sizes
enum class AstSize { Large, Medium, Small };
static constexpr float kAstRadius[3]  = { 38.0f, 22.0f, 12.0f };
static constexpr float kAstMaxSpeed[3]= {  90.0f, 130.0f, 180.0f };
static constexpr int   kAstScore[3]   = { 20, 50, 100 };

// --- Types ----------------------------------------------------------------
struct Vec2 { float x = 0, y = 0; };

static Vec2 add (Vec2 a, Vec2 b) { return { a.x + b.x, a.y + b.y }; }
static Vec2 scl (Vec2 a, float s) { return { a.x * s, a.y * s }; }
static float len(Vec2 a)         { return std::sqrt(a.x * a.x + a.y * a.y); }

// Wrap a position to the screen bounds (negative wraps to far edge).
static void wrap(Vec2& p)
{
    while (p.x <  0)       p.x += WINDOW_W;
    while (p.x >= WINDOW_W) p.x -= WINDOW_W;
    while (p.y <  0)       p.y += WINDOW_H;
    while (p.y >= WINDOW_H) p.y -= WINDOW_H;
}

struct Ship
{
    Vec2  pos;
    Vec2  vel;
    float angle = -1.5708f;    // facing up (-pi/2)
    bool  thrusting = false;
    bool  alive = true;
    float fireCooldown = 0.0f;
};

struct Bullet
{
    Vec2  pos;
    Vec2  vel;
    float lifeRemaining;
};

struct Asteroid
{
    Vec2    pos;
    Vec2    vel;
    AstSize size;
    float   angle = 0;        // visual rotation
    float   spin  = 0;        // rad/sec
};

// --- Game state -----------------------------------------------------------
struct Game
{
    Ship                  ship;
    std::vector<Bullet>   bullets;
    std::vector<Asteroid> asteroids;
    int  score = 0;
    int  lives = START_LIVES;
    bool gameOver = false;
    float respawnTimer = 0.0f;  // when ship dies; respawn delay
};

static float randf(float lo, float hi)
{
    return lo + (hi - lo) * ((float)rand() / (float)RAND_MAX);
}

static Asteroid makeAsteroid(Vec2 pos, AstSize size)
{
    Asteroid a;
    a.pos  = pos;
    float speed = randf(20.0f, kAstMaxSpeed[(int)size]);
    float ang   = randf(0.0f, 6.2832f);
    a.vel  = { std::cos(ang) * speed, std::sin(ang) * speed };
    a.size = size;
    a.spin = randf(-1.5f, 1.5f);
    return a;
}

static void spawnInitialAsteroids(Game& g)
{
    g.asteroids.clear();
    // Place large asteroids at random edges, away from center.
    for (int i = 0; i < START_ASTEROIDS; ++i)
    {
        Vec2 p;
        // Pick an edge: 0=top, 1=right, 2=bottom, 3=left
        int edge = rand() % 4;
        switch (edge)
        {
            case 0: p = { randf(0, WINDOW_W), 0 };          break;
            case 1: p = { (float)WINDOW_W,    randf(0, WINDOW_H) }; break;
            case 2: p = { randf(0, WINDOW_W), (float)WINDOW_H }; break;
            default:p = { 0,                  randf(0, WINDOW_H) }; break;
        }
        g.asteroids.push_back(makeAsteroid(p, AstSize::Large));
    }
}

static void resetShip(Ship& s)
{
    s.pos = { WINDOW_W * 0.5f, WINDOW_H * 0.5f };
    s.vel = { 0, 0 };
    s.angle = -1.5708f;
    s.alive = true;
    s.fireCooldown = 0;
}

static void resetGame(Game& g)
{
    resetShip(g.ship);
    g.bullets.clear();
    spawnInitialAsteroids(g);
    g.score = 0;
    g.lives = START_LIVES;
    g.gameOver = false;
    g.respawnTimer = 0;
}

// --- Collisions -----------------------------------------------------------
static bool circleHit(Vec2 a, float ra, Vec2 b, float rb)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float rr = ra + rb;
    return dx * dx + dy * dy <= rr * rr;
}

// Asteroid splits into smaller pieces (or vanishes if Small).
// Returns the score for hitting this asteroid.
static int splitAsteroid(Game& g, size_t idx)
{
    AstSize sz = g.asteroids[idx].size;
    int score = kAstScore[(int)sz];
    Vec2 p = g.asteroids[idx].pos;

    if (sz == AstSize::Large)
    {
        g.asteroids.push_back(makeAsteroid(p, AstSize::Medium));
        g.asteroids.push_back(makeAsteroid(p, AstSize::Medium));
    }
    else if (sz == AstSize::Medium)
    {
        g.asteroids.push_back(makeAsteroid(p, AstSize::Small));
        g.asteroids.push_back(makeAsteroid(p, AstSize::Small));
    }

    g.asteroids.erase(g.asteroids.begin() + idx);
    return score;
}

// --- Update ---------------------------------------------------------------
static void update(Game& g, float dt, SDL_Window* window)
{
    // Ship
    if (g.ship.alive)
    {
        if (g.ship.thrusting)
        {
            g.ship.vel.x += std::cos(g.ship.angle) * SHIP_THRUST * dt;
            g.ship.vel.y += std::sin(g.ship.angle) * SHIP_THRUST * dt;
        }
        // Drag
        float dragFactor = 1.0f - SHIP_DRAG * dt;
        if (dragFactor < 0) dragFactor = 0;
        g.ship.vel.x *= dragFactor;
        g.ship.vel.y *= dragFactor;

        g.ship.pos = add(g.ship.pos, scl(g.ship.vel, dt));
        wrap(g.ship.pos);

        if (g.ship.fireCooldown > 0) g.ship.fireCooldown -= dt;
    }
    else if (!g.gameOver)
    {
        g.respawnTimer -= dt;
        if (g.respawnTimer <= 0)
            resetShip(g.ship);
    }

    // Bullets
    for (auto& b : g.bullets)
    {
        b.pos = add(b.pos, scl(b.vel, dt));
        wrap(b.pos);
        b.lifeRemaining -= dt;
    }
    g.bullets.erase(
        std::remove_if(g.bullets.begin(), g.bullets.end(),
            [](const Bullet& b){ return b.lifeRemaining <= 0; }),
        g.bullets.end());

    // Asteroids
    for (auto& a : g.asteroids)
    {
        a.pos = add(a.pos, scl(a.vel, dt));
        wrap(a.pos);
        a.angle += a.spin * dt;
    }

    // Bullet-vs-asteroid collisions
    for (size_t bi = 0; bi < g.bullets.size(); ++bi)
    {
        for (size_t ai = 0; ai < g.asteroids.size(); ++ai)
        {
            if (circleHit(g.bullets[bi].pos, 2.0f,
                          g.asteroids[ai].pos, kAstRadius[(int)g.asteroids[ai].size]))
            {
                g.score += splitAsteroid(g, ai);
                g.bullets[bi].lifeRemaining = 0;   // expire on next prune
                break;
            }
        }
    }

    // Ship-vs-asteroid collision
    if (g.ship.alive)
    {
        for (auto& a : g.asteroids)
        {
            if (circleHit(g.ship.pos, SHIP_RADIUS,
                          a.pos, kAstRadius[(int)a.size]))
            {
                g.ship.alive = false;
                g.lives -= 1;
                g.respawnTimer = 1.5f;
                if (g.lives <= 0) g.gameOver = true;
                break;
            }
        }
    }

    // All clear -> respawn a wave.
    if (g.asteroids.empty() && !g.gameOver)
        spawnInitialAsteroids(g);

    // Title bar
    char title[80];
    SDL_snprintf(title, sizeof(title),
        "Vibe Asteroids  -  Score: %d   Lives: %d%s",
        g.score, g.lives, g.gameOver ? "   GAME OVER" : "");
    SDL_SetWindowTitle(window, title);
}

// --- Rendering ------------------------------------------------------------
static void drawShip(SDL_Renderer* r, const Ship& s)
{
    if (!s.alive) return;

    // Three triangle points: nose, left rear, right rear.
    float c = std::cos(s.angle), si = std::sin(s.angle);

    // Local-space points (nose forward along +x in local frame).
    Vec2 nose  { SHIP_RADIUS,        0 };
    Vec2 lrear { -SHIP_RADIUS * 0.8f, -SHIP_RADIUS * 0.7f };
    Vec2 rrear { -SHIP_RADIUS * 0.8f,  SHIP_RADIUS * 0.7f };

    auto toWorld = [&](Vec2 p){
        return Vec2{ s.pos.x + p.x * c - p.y * si,
                     s.pos.y + p.x * si + p.y * c };
    };

    Vec2 n  = toWorld(nose);
    Vec2 lr = toWorld(lrear);
    Vec2 rr = toWorld(rrear);

    SDL_SetRenderDrawColor(r, 230, 230, 240, 255);
    SDL_RenderLine(r, n.x,  n.y,  lr.x, lr.y);
    SDL_RenderLine(r, lr.x, lr.y, rr.x, rr.y);
    SDL_RenderLine(r, rr.x, rr.y, n.x,  n.y);

    // Flame when thrusting.
    if (s.thrusting)
    {
        Vec2 tail   = toWorld({ -SHIP_RADIUS * 0.8f,  0 });
        Vec2 flame  = toWorld({ -SHIP_RADIUS * 1.6f,  0 });
        SDL_SetRenderDrawColor(r, 255, 160,  60, 255);
        SDL_RenderLine(r, tail.x, tail.y, flame.x, flame.y);
    }
}

static void drawAsteroid(SDL_Renderer* r, const Asteroid& a)
{
    // 10-sided polygon with slight per-vertex jitter (deterministic per
    // asteroid via its position so it doesn't shimmer).
    const int sides = 10;
    float radius = kAstRadius[(int)a.size];

    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    Vec2 prev {};
    for (int i = 0; i <= sides; ++i)
    {
        float t = (i % sides) / (float)sides;
        float ang = t * 6.2832f + a.angle;
        // Bumpiness based on the angle (no rand per frame).
        float bump = 0.85f + 0.15f * std::sin(ang * 3.0f);
        Vec2 v {
            a.pos.x + std::cos(ang) * radius * bump,
            a.pos.y + std::sin(ang) * radius * bump
        };
        if (i > 0)
            SDL_RenderLine(r, prev.x, prev.y, v.x, v.y);
        prev = v;
    }
}

static void render(SDL_Renderer* r, const Game& g)
{
    SDL_SetRenderDrawColor(r, 8, 8, 16, 255);
    SDL_RenderClear(r);

    for (const auto& a : g.asteroids) drawAsteroid(r, a);
    for (const auto& b : g.bullets)
    {
        SDL_SetRenderDrawColor(r, 255, 240, 200, 255);
        SDL_FRect rect { b.pos.x - 1.5f, b.pos.y - 1.5f, 3.0f, 3.0f };
        SDL_RenderFillRect(r, &rect);
    }
    drawShip(r, g.ship);

    SDL_RenderPresent(r);
}

// --- main -----------------------------------------------------------------
int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) { SDL_Log("SDL_Init: %s", SDL_GetError()); return 1; }
    srand((unsigned)time(nullptr));

    SDL_Window* window = SDL_CreateWindow("Vibe Asteroids", WINDOW_W, WINDOW_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    Game g;
    resetGame(g);

    bool running = true;
    Uint64 lastTicks = SDL_GetTicksNS();

    while (running)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN)
            {
                if (ev.key.scancode == SDL_SCANCODE_ESCAPE) running = false;
                if (ev.key.scancode == SDL_SCANCODE_R && g.gameOver) resetGame(g);
            }
        }

        const bool* keys = SDL_GetKeyboardState(nullptr);
        const Uint64 now = SDL_GetTicksNS();
        const float dt   = (float)(now - lastTicks) / 1.0e9f;
        lastTicks = now;

        if (g.ship.alive && !g.gameOver)
        {
            if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  g.ship.angle -= SHIP_ROT_SPEED * dt;
            if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) g.ship.angle += SHIP_ROT_SPEED * dt;
            g.ship.thrusting = (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]);

            if (keys[SDL_SCANCODE_SPACE] && g.ship.fireCooldown <= 0)
            {
                Bullet b;
                b.pos = g.ship.pos;
                b.vel = { std::cos(g.ship.angle) * BULLET_SPEED,
                          std::sin(g.ship.angle) * BULLET_SPEED };
                b.lifeRemaining = BULLET_LIFE_S;
                g.bullets.push_back(b);
                g.ship.fireCooldown = FIRE_COOLDOWN_S;
            }
        }
        else
        {
            g.ship.thrusting = false;
        }

        update(g, dt, window);
        render(renderer, g);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
