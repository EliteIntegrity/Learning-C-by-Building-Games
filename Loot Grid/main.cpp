// ============================================================
//  Loot Grid  -  Chapter 15 project
// ============================================================
//
//  A 16x12 grid of cells. The player (green square) moves one
//  cell at a time with WASD. The world is sparsely sprinkled
//  with five kinds of loot, each its own color. Walking onto
//  a loot cell collects the item: it disappears from the
//  world and the player's inventory count goes up.
//
//  Press TAB to dump the current inventory to the Output
//  window. Press R to restart with a fresh world. Escape to
//  quit.
//
//  This is the project from Chapter 15 of "Learning C++ by
//  Building Games". It is built around the std::map and
//  std::unordered_map containers from Chapter 14, plus
//  std::pair and std::vector. There are no classes — that's
//  Chapter 16 onward.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdlib>
#include <ctime>
#include <map>
#include <unordered_map>
#include <utility>     // std::pair
#include <vector>

// --- Configuration --------------------------------------------------------
const int CELL_SIZE   = 50;
const int GRID_COLS   = 16;
const int GRID_ROWS   = 12;
const int SCREEN_W    = CELL_SIZE * GRID_COLS;   // 800
const int SCREEN_H    = CELL_SIZE * GRID_ROWS;   // 600
const int ITEM_COUNT  = 20;

// --- Item types -----------------------------------------------------------
// Scoped enum (enum class) so the names live inside ItemType:: and we
// can't accidentally use one where an int is expected.
enum class ItemType
{
    Coin,
    Gem,
    Key,
    Potion,
    Heart
};

// --- Helpers --------------------------------------------------------------

// Print one item type's friendly name to a SDL_Log call.
const char* itemName(ItemType t)
{
    switch (t)
    {
        case ItemType::Coin:   return "Coin";
        case ItemType::Gem:    return "Gem";
        case ItemType::Key:    return "Key";
        case ItemType::Potion: return "Potion";
        case ItemType::Heart:  return "Heart";
    }
    return "Unknown";
}

// Position on the grid (column, row). Used as a map key.
using Cell = std::pair<int, int>;

// Pick a random ItemType from the five available.
ItemType randomItem()
{
    switch (rand() % 5)
    {
        case 0: return ItemType::Coin;
        case 1: return ItemType::Gem;
        case 2: return ItemType::Key;
        case 3: return ItemType::Potion;
        default: return ItemType::Heart;
    }
}

// Fill the world map with ITEM_COUNT items at random distinct cells,
// avoiding the cell the player is currently on.
void seedWorld(std::map<Cell, ItemType>& world, const Cell& playerCell)
{
    world.clear();
    int placed = 0;
    while (placed < ITEM_COUNT)
    {
        Cell c { rand() % GRID_COLS, rand() % GRID_ROWS };
        if (c == playerCell) continue;
        if (world.find(c) != world.end()) continue;  // already taken
        world[c] = randomItem();
        ++placed;
    }
}

// Reset the player and inventory, then re-seed the world.
void resetGame(Cell& player,
               std::map<ItemType, int>& inventory,
               std::map<Cell, ItemType>& world)
{
    player = { GRID_COLS / 2, GRID_ROWS / 2 };
    inventory.clear();
    seedWorld(world, player);
}

// --- main -----------------------------------------------------------------
int main(int argc, char* argv[])
{
    srand((unsigned)time(nullptr));

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Loot Grid", SCREEN_W, SCREEN_H, 0);
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

    // ItemType -> SDL_Color lookup. unordered_map because we only ever
    // look up by key and don't need any ordering.
    std::unordered_map<ItemType, SDL_Color> itemColors = {
        { ItemType::Coin,   {230, 200,  50, 255} },   // gold
        { ItemType::Gem,    { 80, 220, 220, 255} },   // cyan
        { ItemType::Key,    {220, 220, 220, 255} },   // silver
        { ItemType::Potion, {180,  80, 220, 255} },   // purple
        { ItemType::Heart,  {230,  70,  90, 255} },   // red
    };

    // The world: which cells contain which item. std::map (ordered)
    // so the contents iterate predictably in the debugger.
    std::map<Cell, ItemType> world;

    // The player's inventory: count of each item type collected.
    std::map<ItemType, int> inventory;

    // The player's grid position (column, row).
    Cell player;

    resetGame(player, inventory, world);

    bool running = true;

    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            // We use KEY_DOWN (rising edge) so each press moves exactly
            // one cell, instead of skating across the grid every frame.
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                    running = false;

                Cell next = player;

                switch (event.key.scancode)
                {
                    case SDL_SCANCODE_W: next.second -= 1; break;
                    case SDL_SCANCODE_S: next.second += 1; break;
                    case SDL_SCANCODE_A: next.first  -= 1; break;
                    case SDL_SCANCODE_D: next.first  += 1; break;

                    case SDL_SCANCODE_TAB:
                        SDL_Log("--- Inventory ---");
                        if (inventory.empty())
                        {
                            SDL_Log("(nothing yet)");
                        }
                        else
                        {
                            for (const auto& [type, count] : inventory)
                                SDL_Log("  %-7s x %d", itemName(type), count);
                        }
                        SDL_Log("Items remaining in world: %d", (int)world.size());
                        break;

                    case SDL_SCANCODE_R:
                        SDL_Log("--- New game ---");
                        resetGame(player, inventory, world);
                        break;

                    default:
                        break;
                }

                // Clamp the candidate move to the grid.
                if (next.first  < 0)            next.first  = 0;
                if (next.first  >= GRID_COLS)   next.first  = GRID_COLS - 1;
                if (next.second < 0)            next.second = 0;
                if (next.second >= GRID_ROWS)   next.second = GRID_ROWS - 1;

                // Commit the move.
                player = next;

                // Did we land on an item?
                auto it = world.find(player);
                if (it != world.end())
                {
                    ItemType picked = it->second;
                    inventory[picked] += 1;   // creates the entry if missing
                    world.erase(it);
                    SDL_Log("Picked up a %s  (have %d)",
                            itemName(picked), inventory[picked]);

                    if (world.empty())
                        SDL_Log("All loot collected! Press R for a new world.");
                }
            }
        }

        // -- Render --
        SDL_SetRenderDrawColor(renderer, 25, 25, 30, 255);
        SDL_RenderClear(renderer);

        // Grid lines (subtle, just to make cells visible).
        SDL_SetRenderDrawColor(renderer, 45, 45, 55, 255);
        for (int c = 0; c <= GRID_COLS; ++c)
            SDL_RenderLine(renderer,
                           (float)(c * CELL_SIZE), 0.0f,
                           (float)(c * CELL_SIZE), (float)SCREEN_H);
        for (int r = 0; r <= GRID_ROWS; ++r)
            SDL_RenderLine(renderer,
                           0.0f,              (float)(r * CELL_SIZE),
                           (float)SCREEN_W,   (float)(r * CELL_SIZE));

        // Items.
        for (const auto& [cell, type] : world)
        {
            SDL_Color col = itemColors[type];
            SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
            SDL_FRect r = {
                (float)(cell.first  * CELL_SIZE + 12),
                (float)(cell.second * CELL_SIZE + 12),
                (float)(CELL_SIZE - 24),
                (float)(CELL_SIZE - 24)
            };
            SDL_RenderFillRect(renderer, &r);
        }

        // Player.
        SDL_SetRenderDrawColor(renderer, 80, 220, 100, 255);
        SDL_FRect pRect = {
            (float)(player.first  * CELL_SIZE + 6),
            (float)(player.second * CELL_SIZE + 6),
            (float)(CELL_SIZE - 12),
            (float)(CELL_SIZE - 12)
        };
        SDL_RenderFillRect(renderer, &pRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
