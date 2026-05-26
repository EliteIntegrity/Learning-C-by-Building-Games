// Block Swarm
// 1000 small blocks bounce around the screen, reversing direction when they
// hit a wall.  Uses SDL3 for the window and drawing.

#include <SDL3/SDL.h>       // main SDL3 header - window, renderer, events
#include <SDL3/SDL_main.h>  // lets SDL redirect WinMain -> main on Windows
#include <vector>           // std::vector - a resizable array
#include <cstdlib>          // rand(), srand()
#include <ctime>            // time() - used to seed the random number generator

// ---------------------------------------------------------------------------
// Constants - change these to tweak the look and feel of the simulation
// ---------------------------------------------------------------------------

const int WINDOW_WIDTH  = 1600;   // width of the window in pixels
const int WINDOW_HEIGHT = 1200;   // height of the window in pixels

const int NUM_BLOCKS  = 100;    // how many blocks are in the swarm
const int BLOCK_SIZE  = 6;       // width AND height of each block in pixels

const float MIN_SPEED = 0.01f;    // slowest a block can move (pixels per frame)
const float MAX_SPEED = 0.1f;    // fastest a block can move (pixels per frame)

// ---------------------------------------------------------------------------
// Block struct
// A "struct" groups related pieces of data together under one name.
// Each block needs to know: where it is (x, y) and how fast it is moving
// in each direction (velX, velY).  Positive velX = moving right,
// negative velX = moving left.  Same idea vertically for velY.
// ---------------------------------------------------------------------------
struct Block
{
    float x;    // horizontal position of the block's top-left corner
    float y;    // vertical   position of the block's top-left corner
    float velX; // horizontal velocity in pixels per frame
    float velY; // vertical   velocity in pixels per frame
};

// ---------------------------------------------------------------------------
// Helper: returns a random float between lo and hi (inclusive)
// ---------------------------------------------------------------------------
float randomFloat(float lo, float hi)
{
    // rand() gives an integer 0..RAND_MAX.  Dividing by RAND_MAX gives 0..1.
    // Multiply by the range and add the lower bound to shift into [lo, hi].
    return lo + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (hi - lo);
}

// ---------------------------------------------------------------------------
// main - program entry point
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Seed the random number generator with the current time so the block
    // positions are different every time the program runs.
    srand(static_cast<unsigned int>(time(nullptr)));

    // -----------------------------------------------------------------------
    // 1. Initialise SDL
    //    SDL_Init sets up the library.  SDL_INIT_VIDEO is all we need because
    //    we only want a window and renderer (no audio, no joystick, etc.).
    //    In SDL3, SDL_Init returns true on success, false on failure.
    // -----------------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;  // exit the program with an error code
    }

    // -----------------------------------------------------------------------
    // 2. Create the window
    // -----------------------------------------------------------------------
    SDL_Window* window = SDL_CreateWindow(
        "Block Swarm",   // text shown in the title bar
        WINDOW_WIDTH,    // window width  in pixels
        WINDOW_HEIGHT,   // window height in pixels
        0                // flags: 0 means a plain, default window
    );

    if (window == nullptr)   // nullptr means SDL failed to create the window
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // -----------------------------------------------------------------------
    // 3. Create the renderer
    //    The renderer is the object that actually draws things on the window.
    //    nullptr as the second argument tells SDL to pick the best driver
    //    automatically (usually hardware-accelerated via DirectX or OpenGL).
    // -----------------------------------------------------------------------
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (renderer == nullptr)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // -----------------------------------------------------------------------
    // 4. Create the swarm
    //    We use a std::vector<Block> to store all 1000 blocks in one place.
    //    A vector is like an array but it can grow as we push_back() items.
    // -----------------------------------------------------------------------
    std::vector<Block> blocks;  // starts empty

    for (int i = 0; i < NUM_BLOCKS; ++i)
    {
        Block b;

        // Random starting position.  We subtract BLOCK_SIZE so the block
        // starts fully inside the window (not hanging off the right/bottom edge).
        b.x = randomFloat(0.0f, static_cast<float>(WINDOW_WIDTH  - BLOCK_SIZE));
        b.y = randomFloat(0.0f, static_cast<float>(WINDOW_HEIGHT - BLOCK_SIZE));

        // Random speed between MIN_SPEED and MAX_SPEED
        b.velX = randomFloat(MIN_SPEED, MAX_SPEED);
        b.velY = randomFloat(MIN_SPEED, MAX_SPEED);

        // Randomly flip the direction so about half the blocks start moving
        // left instead of right, and half start moving up instead of down.
        if (rand() % 2 == 0) b.velX = -b.velX;
        if (rand() % 2 == 0) b.velY = -b.velY;

        blocks.push_back(b);  // add this block to the end of the vector
    }

    // -----------------------------------------------------------------------
    // 5. Main game loop
    //    The loop repeats every frame.  Each frame we:
    //      a) check for events (e.g. the user closing the window)
    //      b) update the simulation (move blocks, bounce off walls)
    //      c) draw everything to the screen
    // -----------------------------------------------------------------------
    bool running = true;   // when this becomes false the loop ends

    while (running)
    {
        // -------------------------------------------------------------------
        // 5a. Event handling
        //     SDL queues up events (key presses, mouse clicks, window close,
        //     etc.).  SDL_PollEvent fetches one event at a time and returns
        //     false when the queue is empty.
        // -------------------------------------------------------------------
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)  // user clicked the X button
            {
                running = false;
            }
        }

        // -------------------------------------------------------------------
        // 5b. Update - move every block and bounce it off the walls
        // -------------------------------------------------------------------
        for (int i = 0; i < static_cast<int>(blocks.size()); ++i)
        {
            // Move the block by adding its velocity to its position.
            // velX/velY are measured in pixels per frame.
            blocks[i].x += blocks[i].velX;
            blocks[i].y += blocks[i].velY;

            // --- Bounce off the LEFT wall ---
            if (blocks[i].x < 0.0f)
            {
                blocks[i].x    = 0.0f;              // push it back inside
                blocks[i].velX = -blocks[i].velX;   // reverse horizontal direction
            }

            // --- Bounce off the RIGHT wall ---
            // The right edge of the block is x + BLOCK_SIZE, so we compare
            // that against the window width.
            if (blocks[i].x + BLOCK_SIZE > WINDOW_WIDTH)
            {
                blocks[i].x    = static_cast<float>(WINDOW_WIDTH - BLOCK_SIZE);
                blocks[i].velX = -blocks[i].velX;
            }

            // --- Bounce off the TOP wall ---
            if (blocks[i].y < 0.0f)
            {
                blocks[i].y    = 0.0f;
                blocks[i].velY = -blocks[i].velY;   // reverse vertical direction
            }

            // --- Bounce off the BOTTOM wall ---
            if (blocks[i].y + BLOCK_SIZE > WINDOW_HEIGHT)
            {
                blocks[i].y    = static_cast<float>(WINDOW_HEIGHT - BLOCK_SIZE);
                blocks[i].velY = -blocks[i].velY;
            }
        }

        // -------------------------------------------------------------------
        // 5c. Render - draw the current frame
        // -------------------------------------------------------------------

        // Paint the whole window black before drawing anything
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // R G B A
        SDL_RenderClear(renderer);

        // Draw every block as a solid white rectangle
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        for (int i = 0; i < static_cast<int>(blocks.size()); ++i)
        {
            // SDL3 uses SDL_FRect (float rectangle) when drawing.
            // x and y are the top-left corner; w and h are width and height.
            SDL_FRect rect;
            rect.x = blocks[i].x;
            rect.y = blocks[i].y;
            rect.w = static_cast<float>(BLOCK_SIZE);
            rect.h = static_cast<float>(BLOCK_SIZE);

            SDL_RenderFillRect(renderer, &rect);  // fill the rectangle solid
        }

        // Show the finished frame (SDL draws to a back buffer; this swaps it
        // to the screen so the user actually sees it).
        SDL_RenderPresent(renderer);
    }

    // -----------------------------------------------------------------------
    // 6. Clean up
    //    Always destroy SDL objects in reverse order of creation, then call
    //    SDL_Quit to release everything SDL set up internally.
    // -----------------------------------------------------------------------
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;  // 0 = program finished successfully
}
