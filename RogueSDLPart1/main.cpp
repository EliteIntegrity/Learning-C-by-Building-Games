// ============================================================
//  Rogue SDL  -  Chapter 28 entry point
// ============================================================
//
//  Window + SDL + SDL_ttf setup, then we hand off to Game.
//  The Game constructor takes ownership of the font, glyph
//  cache, and the rest of the world.
//
//  See README.md for the controls and the list of files.
// ============================================================

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Common.h"
#include "Game.h"

int main(int argc, char* argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    if (!TTF_Init())
    {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Rogue SDL", WINDOW_W, WINDOW_H, 0);
    if (!window)
    {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    // Scope block so Game and its members are destroyed BEFORE we
    // tear down the renderer / window / SDL itself.
    {
        Game game(renderer);
        game.newDungeon();
        game.run();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
