// ============================================================
//  Vibe Snake  -  Chapter 24 vibe-coded project
// ============================================================
//
//  Classic snake. 800x600 window, 20px grid, classic "eat the
//  apple, grow, don't crash" gameplay.
//
//  This is the version I ended up with after the prompt-and-
//  reaction conversation in Chapter 24. Yours will be
//  different — that's the whole point.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <cstdlib>
#include <ctime>

// --- Configuration --------------------------------------------------------
static constexpr int   CELL_PX       = 20;
static constexpr int   GRID_W        = 40;
static constexpr int   GRID_H        = 30;
static constexpr int   WINDOW_W      = CELL_PX * GRID_W;     // 800
static constexpr int   WINDOW_H      = CELL_PX * GRID_H;     // 600

static constexpr Uint64 START_TICK_MS = 125;   // initial move interval
static constexpr Uint64 MIN_TICK_MS   =  60;   // cap on how fast we get

struct Cell { int x; int y; };

static bool sameCell(Cell a, Cell b) { return a.x == b.x && a.y == b.y; }

// --- Game state -----------------------------------------------------------
struct Game
{
    std::vector<Cell> snake;     // [0] is the head
    Cell              dir;        // direction the head is moving
    Cell              pendingDir; // direction queued from the last input
    Cell              apple;
    int               score    = 0;
    bool              gameOver = false;
    Uint64            lastMove = 0;
    Uint64            tickMs   = START_TICK_MS;
};

static Cell randomEmptyCell(const std::vector<Cell>& occupied)
{
    // Pick a random cell that doesn't intersect the snake.
    for (int tries = 0; tries < 1000; ++tries)
    {
        Cell c { rand() % GRID_W, rand() % GRID_H };
        bool occupiedHere = false;
        for (Cell s : occupied) if (sameCell(s, c)) { occupiedHere = true; break; }
        if (!occupiedHere) return c;
    }
    return Cell{ 0, 0 };
}

static void resetGame(Game& g)
{
    g.snake.clear();
    int cx = GRID_W / 2;
    int cy = GRID_H / 2;
    g.snake.push_back({ cx,     cy });   // head
    g.snake.push_back({ cx - 1, cy });
    g.snake.push_back({ cx - 2, cy });

    g.dir        = { 1, 0 };   // moving right
    g.pendingDir = g.dir;
    g.apple      = randomEmptyCell(g.snake);
    g.score      = 0;
    g.gameOver   = false;
    g.lastMove   = SDL_GetTicks();
    g.tickMs     = START_TICK_MS;
}

static void queueDir(Game& g, int dx, int dy)
{
    // Don't allow reversing into the snake's own neck.
    if (dx == -g.dir.x && dy == -g.dir.y) return;
    g.pendingDir = { dx, dy };
}

static void stepSnake(Game& g, SDL_Window* window)
{
    if (g.gameOver) return;

    g.dir = g.pendingDir;

    Cell newHead { g.snake.front().x + g.dir.x, g.snake.front().y + g.dir.y };

    // Walls.
    if (newHead.x < 0 || newHead.x >= GRID_W ||
        newHead.y < 0 || newHead.y >= GRID_H)
    {
        g.gameOver = true;
        return;
    }
    // Self-collision. Skip the very last segment because it's about to move.
    for (size_t i = 0; i + 1 < g.snake.size(); ++i)
        if (sameCell(g.snake[i], newHead)) { g.gameOver = true; return; }

    g.snake.insert(g.snake.begin(), newHead);

    if (sameCell(newHead, g.apple))
    {
        ++g.score;
        // Speed up slightly per apple, capped.
        if (g.tickMs > MIN_TICK_MS) g.tickMs -= 5;
        g.apple = randomEmptyCell(g.snake);

        char title[64];
        SDL_snprintf(title, sizeof(title), "Vibe Snake  -  Score: %d", g.score);
        SDL_SetWindowTitle(window, title);
    }
    else
    {
        g.snake.pop_back();   // didn't eat — tail follows
    }
}

static void render(SDL_Renderer* r, const Game& g)
{
    // Background
    SDL_SetRenderDrawColor(r, 16, 16, 20, 255);
    SDL_RenderClear(r);

    // Subtle grid lines
    SDL_SetRenderDrawColor(r, 28, 28, 36, 255);
    for (int x = 0; x <= GRID_W; ++x)
        SDL_RenderLine(r, (float)(x * CELL_PX), 0.0f,
                          (float)(x * CELL_PX), (float)WINDOW_H);
    for (int y = 0; y <= GRID_H; ++y)
        SDL_RenderLine(r, 0.0f, (float)(y * CELL_PX),
                          (float)WINDOW_W, (float)(y * CELL_PX));

    // Apple
    SDL_SetRenderDrawColor(r, 230, 60, 60, 255);
    SDL_FRect aRect {
        (float)(g.apple.x * CELL_PX + 2),
        (float)(g.apple.y * CELL_PX + 2),
        (float)(CELL_PX - 4), (float)(CELL_PX - 4)
    };
    SDL_RenderFillRect(r, &aRect);

    // Snake — head a bit brighter than the body.
    for (size_t i = 0; i < g.snake.size(); ++i)
    {
        bool isHead = (i == 0);
        SDL_SetRenderDrawColor(r,
            isHead ? 120 :  80,
            isHead ? 230 : 180,
            isHead ? 120 :  90,
            255);
        SDL_FRect cell {
            (float)(g.snake[i].x * CELL_PX + 1),
            (float)(g.snake[i].y * CELL_PX + 1),
            (float)(CELL_PX - 2), (float)(CELL_PX - 2)
        };
        SDL_RenderFillRect(r, &cell);
    }

    // Game-over overlay
    if (g.gameOver)
    {
        SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(r, 0, 0, 0, 160);
        SDL_FRect full { 0, 0, (float)WINDOW_W, (float)WINDOW_H };
        SDL_RenderFillRect(r, &full);
    }

    SDL_RenderPresent(r);
}

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    srand((unsigned)time(nullptr));

    SDL_Window* window = SDL_CreateWindow("Vibe Snake", WINDOW_W, WINDOW_H, 0);
    if (!window) { SDL_Log("CreateWindow: %s", SDL_GetError()); SDL_Quit(); return 1; }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) { SDL_DestroyWindow(window); SDL_Quit(); return 1; }

    Game g;
    resetGame(g);

    bool running = true;
    while (running)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT) { running = false; }
            else if (ev.type == SDL_EVENT_KEY_DOWN)
            {
                switch (ev.key.scancode)
                {
                    case SDL_SCANCODE_ESCAPE: running = false; break;

                    case SDL_SCANCODE_W: case SDL_SCANCODE_UP:    queueDir(g,  0, -1); break;
                    case SDL_SCANCODE_S: case SDL_SCANCODE_DOWN:  queueDir(g,  0, +1); break;
                    case SDL_SCANCODE_A: case SDL_SCANCODE_LEFT:  queueDir(g, -1,  0); break;
                    case SDL_SCANCODE_D: case SDL_SCANCODE_RIGHT: queueDir(g, +1,  0); break;

                    case SDL_SCANCODE_R:
                        if (g.gameOver)
                        {
                            resetGame(g);
                            SDL_SetWindowTitle(window, "Vibe Snake  -  Score: 0");
                        }
                        break;
                    default: break;
                }
            }
        }

        Uint64 now = SDL_GetTicks();
        if (now - g.lastMove >= g.tickMs)
        {
            stepSnake(g, window);
            g.lastMove = now;
        }

        render(renderer, g);
        SDL_Delay(8);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
