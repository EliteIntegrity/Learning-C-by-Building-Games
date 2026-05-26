#include "Game.h"
#include "FOV.h"
#include "SaveLoad.h"

static const char* SAVE_PATH = "rogue_sdl_save.txt";
static const char* FONT_PATH = "RobotoMono-Light.ttf";

Game::Game(SDL_Renderer* renderer)
    : renderer_(renderer)
    , glyphs_(renderer, FONT_PATH, 18)
    , map_(MAP_W, MAP_H)
    , gen_()
    , player_(nullptr)
{
}

void Game::newDungeon()
{
    Point start = gen_.generate(map_);
    player_ = std::make_unique<Player>(start);
    recomputeFov();
    dirty_ = true;
}

void Game::recomputeFov()
{
    FOV::compute(map_, player_->position(), FOV_RADIUS);
}

void Game::save()
{
    if (SaveLoad::save(SAVE_PATH, map_, *player_))
        SDL_Log("Saved to %s", SAVE_PATH);
    else
        SDL_Log("Save FAILED");
}

void Game::load()
{
    if (SaveLoad::load(SAVE_PATH, map_, *player_))
    {
        recomputeFov();
        dirty_ = true;
        SDL_Log("Loaded from %s", SAVE_PATH);
    }
    else
    {
        SDL_Log("Load FAILED");
    }
}

bool Game::handleEvent(const SDL_Event& ev)
{
    if (ev.type == SDL_EVENT_QUIT) { quit_ = true; return false; }

    if (ev.type != SDL_EVENT_KEY_DOWN) return false;

    int dx = 0, dy = 0;
    switch (ev.key.scancode)
    {
        case SDL_SCANCODE_ESCAPE: quit_ = true; return false;

        case SDL_SCANCODE_W: case SDL_SCANCODE_UP:    dy = -1; break;
        case SDL_SCANCODE_S: case SDL_SCANCODE_DOWN:  dy = +1; break;
        case SDL_SCANCODE_A: case SDL_SCANCODE_LEFT:  dx = -1; break;
        case SDL_SCANCODE_D: case SDL_SCANCODE_RIGHT: dx = +1; break;

        case SDL_SCANCODE_PERIOD:
            // '>' usually requires shift on most keyboards; we accept
            // a bare period as "descend on the stairs" so it works
            // without thinking about modifier keys.
            if (map_.at(player_->position().x,
                        player_->position().y).terrain == Terrain::StairsDown)
            {
                newDungeon();
                return true;
            }
            return false;

        case SDL_SCANCODE_F5: save(); return false;
        case SDL_SCANCODE_F9: load(); return false;

        default: return false;
    }

    if (dx == 0 && dy == 0) return false;

    if (player_->tryMove(dx, dy, map_))
    {
        recomputeFov();
        return true;
    }
    return false;
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer_,
        Palette::BG.r, Palette::BG.g, Palette::BG.b, 255);
    SDL_RenderClear(renderer_);

    map_.render(renderer_, glyphs_);

    // The player is the only entity in Chapter 28. Chapter 29
    // will iterate a vector of unique_ptr<Entity> here.
    if (player_)
        player_->render(renderer_, glyphs_);

    SDL_RenderPresent(renderer_);
}

void Game::run()
{
    // The turn-based main loop. Note SDL_WaitEvent (not SDL_PollEvent):
    // we block here using ~zero CPU until the player does something.
    while (!quit_)
    {
        if (dirty_)
        {
            render();
            dirty_ = false;
        }

        SDL_Event ev;
        if (!SDL_WaitEvent(&ev)) break;

        bool tookTurn = handleEvent(ev);

        // Anything that took a turn is a reason to redraw.
        // Window-expose events also need a redraw.
        if (tookTurn || ev.type == SDL_EVENT_WINDOW_EXPOSED)
            dirty_ = true;
    }
}
