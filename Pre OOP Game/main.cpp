/*
    Pre-OOP Side-Scrolling Shooter (SDL3 Tutorial)
    -----------------------------------------------
    A small example game built for an introductory SDL3 tutorial.
    Classes are intentionally NOT used (we use plain structs and free
    functions) so it can be taught before object-oriented programming
    is introduced.

    Gameplay:
        * The player ship sits on the left side of the screen.
        * WASD moves the ship around inside the play area.
        * SPACE fires a laser (drawn as a line primitive). At most 3
          lasers can be on the screen at once.
        * Alien rabbits spawn off-screen on the right at random
          heights and scroll left toward the player.
        * Hitting a rabbit with a laser scores 10 points.
        * Colliding with a rabbit costs the player one life.
        * The player has 3 lives. When all are lost the game ends and
          can be restarted with R.

    Demonstrates:
        - SDL3 window / renderer creation
        - Loading PNG textures with SDL3_image
        - Drawing sprites and line primitives
        - Loading a TTF font and rendering text with SDL3_ttf
        - Loading and playing WAV sound effects with SDL3 audio streams
        - Keyboard input with SDL_GetKeyboardState and SDL_PollEvent
        - Simple AABB collision detection
*/

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

// --- Configuration constants -------------------------------------------
// Putting "magic numbers" up here makes them easy to tweak.
const int   SCREEN_W       = 1024;
const int   SCREEN_H       = 600;

const float PLAYER_SPEED   = 5.0f;     // pixels per frame
const float LASER_SPEED    = 12.0f;    // pixels per frame
const float RABBIT_MIN_SPD = 2.0f;
const float RABBIT_MAX_SPD = 5.0f;

const int   MAX_LASERS     = 3;        // lasers allowed on screen at once
const int   MAX_RABBITS    = 12;       // size of the rabbit pool
const int   START_LIVES    = 3;
const int   POINTS_PER_HIT = 10;

// --- Game data structures ----------------------------------------------
// Simple POD structs - no constructors, no methods, just data.
struct Player {
    float x, y;        // top-left position
    float w, h;        // size in pixels
    int   lives;
};

struct Laser {
    float x, y;        // tip of the laser line
    bool  active;      // whether this slot is in use
};

struct Rabbit {
    float x, y;
    float w, h;
    float speed;       // horizontal speed (moves left)
    bool  active;
};

// A loaded sound effect plus the audio stream we push it through.
// Each sound has its own stream so they can play independently.
struct Sound {
    SDL_AudioStream* stream;
    Uint8*           buf;
    Uint32           len;
};

// --- Small helper functions --------------------------------------------

// Axis-aligned bounding box overlap test.
bool rectsOverlap(float ax, float ay, float aw, float ah,
                  float bx, float by, float bw, float bh) {
    return ax < bx + bw && ax + aw > bx &&
           ay < by + bh && ay + ah > by;
}

// Return a random float in [lo, hi].
float randRange(float lo, float hi) {
    float t = (float)rand() / (float)RAND_MAX;
    return lo + t * (hi - lo);
}

// Load a WAV file and wire it up to an audio device stream so that
// calling playSound() will mix it onto the default playback device.
Sound loadSound(const char* path) {
    Sound s = { nullptr, nullptr, 0 };
    SDL_AudioSpec spec;
    if (!SDL_LoadWAV(path, &spec, &s.buf, &s.len)) {
        SDL_Log("Failed to load WAV %s: %s", path, SDL_GetError());
        return s;
    }
    // A NULL callback means we'll feed the stream manually with PutData.
    s.stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (s.stream) {
        // Streams start paused; resume so audio actually plays.
        SDL_ResumeAudioStreamDevice(s.stream);
    } else {
        SDL_Log("Failed to open audio stream for %s: %s", path, SDL_GetError());
    }
    return s;
}

// Trigger a sound. Clearing first means rapid repeats restart the clip
// instead of queuing up a long backlog.
void playSound(const Sound& s) {
    if (!s.stream) return;
    SDL_ClearAudioStream(s.stream);
    SDL_PutAudioStreamData(s.stream, s.buf, (int)s.len);
}

// Render a UTF-8 string to a fresh texture. Caller must SDL_DestroyTexture.
SDL_Texture* makeTextTexture(SDL_Renderer* r, TTF_Font* f,
                             const char* text, SDL_Color color,
                             int* outW, int* outH) {
    SDL_Surface* surf = TTF_RenderText_Blended(f, text, 0, color);
    if (!surf) return nullptr;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    if (outW) *outW = surf->w;
    if (outH) *outH = surf->h;
    SDL_DestroySurface(surf);
    return tex;
}

// Convenience: build texture, draw at (x, y), throw it away.
// Fine for a tutorial; in a real game you'd cache these.
void drawText(SDL_Renderer* r, TTF_Font* f, const char* text,
              int x, int y, SDL_Color color) {
    int w = 0, h = 0;
    SDL_Texture* tex = makeTextTexture(r, f, text, color, &w, &h);
    if (!tex) return;
    SDL_FRect dst = { (float)x, (float)y, (float)w, (float)h };
    SDL_RenderTexture(r, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

// Spawn a rabbit just off the right edge at a random height/speed.
void spawnRabbit(Rabbit& rb) {
    rb.w = 48.0f;
    rb.h = 48.0f;
    rb.x = (float)SCREEN_W + randRange(0.0f, 200.0f); // stagger spawns
    rb.y = randRange(0.0f, (float)SCREEN_H - rb.h);
    rb.speed = randRange(RABBIT_MIN_SPD, RABBIT_MAX_SPD);
    rb.active = true;
}

// Reset all game state for a fresh run.
void resetGame(Player& p, Laser lasers[], Rabbit rabbits[], int& score) {
    p.w = 64.0f;
    p.h = 48.0f;
    p.x = 40.0f;
    p.y = (SCREEN_H - p.h) * 0.5f;
    p.lives = START_LIVES;

    for (int i = 0; i < MAX_LASERS; ++i)  lasers[i].active = false;
    for (int i = 0; i < MAX_RABBITS; ++i) rabbits[i].active = false;
    score = 0;
}

// =======================================================================
//                                 MAIN
// =======================================================================
int main(int argc, char* argv[]) {
    // --- Initialize SDL subsystems -------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        return 1;
    }

    // Window + renderer. Passing NULL for the renderer driver lets SDL pick.
    SDL_Window*   window   = SDL_CreateWindow("Pre-OOP Shooter",
                                              SCREEN_W, SCREEN_H, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!window || !renderer) {
        SDL_Log("Failed to create window/renderer: %s", SDL_GetError());
        return 1;
    }

    // --- Load assets ----------------------------------------------------
    SDL_Texture* playerTex = IMG_LoadTexture(renderer, "player_ship.png");
    SDL_Texture* rabbitTex = IMG_LoadTexture(renderer, "alien_rabbit.png");
    TTF_Font*    font      = TTF_OpenFont("RobotoMono-Light.ttf", 22);

    // Note: the shoot wav is named "shooot.wav" (three o's) in the project folder.
    Sound shootSnd = loadSound("shooot.wav");
    Sound hitSnd   = loadSound("hit_alien.wav");
    Sound crashSnd = loadSound("player_crash.wav");

    if (!playerTex || !rabbitTex || !font) {
        SDL_Log("Failed to load an asset: %s", SDL_GetError());
        return 1;
    }

    // --- Game state -----------------------------------------------------
    srand((unsigned)time(nullptr));

    Player player;
    Laser  lasers[MAX_LASERS];
    Rabbit rabbits[MAX_RABBITS];
    int    score     = 0;
    int    highScore = 0;
    bool   gameOver  = false;

    resetGame(player, lasers, rabbits, score);

    // Used to throttle space-bar firing so holding it doesn't empty the
    // pool instantly. We track whether space was already down last frame.
    bool spaceWasDown = false;

    // --- Main loop ------------------------------------------------------
    bool running = true;
    while (running) {
        // -- Event handling --
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (ev.type == SDL_EVENT_KEY_DOWN) {
                // Restart on R when game is over.
                if (gameOver && ev.key.key == SDLK_R) {
                    resetGame(player, lasers, rabbits, score);
                    gameOver = false;
                }
                if (ev.key.key == SDLK_ESCAPE) running = false;
            }
        }

        // SDL_GetKeyboardState returns the current "is-down" state for
        // every key; great for smooth movement input.
        const bool* keys = SDL_GetKeyboardState(nullptr);

        if (!gameOver) {
            // -- Player movement (WASD) --
            if (keys[SDL_SCANCODE_W]) player.y -= PLAYER_SPEED;
            if (keys[SDL_SCANCODE_S]) player.y += PLAYER_SPEED;
            if (keys[SDL_SCANCODE_A]) player.x -= PLAYER_SPEED;
            if (keys[SDL_SCANCODE_D]) player.x += PLAYER_SPEED;

            // Clamp player inside the screen.
            if (player.x < 0) player.x = 0;
            if (player.y < 0) player.y = 0;
            if (player.x + player.w > SCREEN_W) player.x = SCREEN_W - player.w;
            if (player.y + player.h > SCREEN_H) player.y = SCREEN_H - player.h;

            // -- Fire laser on rising edge of SPACE --
            bool spaceDown = keys[SDL_SCANCODE_SPACE];
            if (spaceDown && !spaceWasDown) {
                // Find the first inactive slot in the pool.
                for (int i = 0; i < MAX_LASERS; ++i) {
                    if (!lasers[i].active) {
                        lasers[i].active = true;
                        lasers[i].x = player.x + player.w;          // nose of ship
                        lasers[i].y = player.y + player.h * 0.5f;   // mid-height
                        playSound(shootSnd);
                        break;
                    }
                }
            }
            spaceWasDown = spaceDown;

            // -- Update lasers --
            for (int i = 0; i < MAX_LASERS; ++i) {
                if (!lasers[i].active) continue;
                lasers[i].x += LASER_SPEED;
                if (lasers[i].x > SCREEN_W) lasers[i].active = false;
            }

            // -- Spawn rabbits to keep the pool full --
            // Each frame, find one inactive rabbit and re-spawn it.
            for (int i = 0; i < MAX_RABBITS; ++i) {
                if (!rabbits[i].active) {
                    spawnRabbit(rabbits[i]);
                    break;
                }
            }

            // -- Update rabbits --
            for (int i = 0; i < MAX_RABBITS; ++i) {
                if (!rabbits[i].active) continue;
                rabbits[i].x -= rabbits[i].speed;
                if (rabbits[i].x + rabbits[i].w < 0) {
                    rabbits[i].active = false;     // off the left edge
                }
            }

            // -- Collision: lasers vs rabbits --
            for (int i = 0; i < MAX_LASERS; ++i) {
                if (!lasers[i].active) continue;
                // Treat the laser as a tiny rectangle for the test.
                float lx = lasers[i].x - 16.0f;   // 16px long beam
                float ly = lasers[i].y - 1.0f;
                float lw = 16.0f, lh = 2.0f;

                for (int j = 0; j < MAX_RABBITS; ++j) {
                    if (!rabbits[j].active) continue;
                    if (rectsOverlap(lx, ly, lw, lh,
                                     rabbits[j].x, rabbits[j].y,
                                     rabbits[j].w, rabbits[j].h)) {
                        rabbits[j].active = false;
                        lasers[i].active  = false;
                        score += POINTS_PER_HIT;
                        playSound(hitSnd);
                        break;
                    }
                }
            }

            // -- Collision: player vs rabbits --
            for (int j = 0; j < MAX_RABBITS; ++j) {
                if (!rabbits[j].active) continue;
                if (rectsOverlap(player.x, player.y, player.w, player.h,
                                 rabbits[j].x, rabbits[j].y,
                                 rabbits[j].w, rabbits[j].h)) {
                    rabbits[j].active = false;
                    player.lives--;
                    playSound(crashSnd);
                    if (player.lives <= 0) {
                        gameOver = true;
                        if (score > highScore) highScore = score;
                    }
                }
            }
        }

        // --------------- Rendering --------------------------------------
        // Clear to a dark space-blue.
        SDL_SetRenderDrawColor(renderer, 10, 10, 30, 255);
        SDL_RenderClear(renderer);

        // Player ship.
        SDL_FRect playerDst = { player.x, player.y, player.w, player.h };
        SDL_RenderTexture(renderer, playerTex, nullptr, &playerDst);

        // Lasers as bright green line primitives.
        SDL_SetRenderDrawColor(renderer, 80, 255, 80, 255);
        for (int i = 0; i < MAX_LASERS; ++i) {
            if (!lasers[i].active) continue;
            SDL_RenderLine(renderer,
                           lasers[i].x - 16.0f, lasers[i].y,
                           lasers[i].x,         lasers[i].y);
        }

        // Rabbits.
        for (int i = 0; i < MAX_RABBITS; ++i) {
            if (!rabbits[i].active) continue;
            SDL_FRect rdst = { rabbits[i].x, rabbits[i].y,
                               rabbits[i].w, rabbits[i].h };
            SDL_RenderTexture(renderer, rabbitTex, nullptr, &rdst);
        }

        // HUD text (lives / score / high score).
        SDL_Color white = { 255, 255, 255, 255 };
        char buf[64];
        SDL_snprintf(buf, sizeof(buf), "Lives: %d", player.lives);
        drawText(renderer, font, buf, 10, 8, white);

        SDL_snprintf(buf, sizeof(buf), "Score: %d", score);
        drawText(renderer, font, buf, 200, 8, white);

        SDL_snprintf(buf, sizeof(buf), "High: %d", highScore);
        drawText(renderer, font, buf, 400, 8, white);

        // Game-over overlay.
        if (gameOver) {
            SDL_Color yellow = { 255, 230, 80, 255 };
            drawText(renderer, font, "GAME OVER",
                     SCREEN_W / 2 - 60, SCREEN_H / 2 - 30, yellow);
            drawText(renderer, font, "Press R to restart",
                     SCREEN_W / 2 - 110, SCREEN_H / 2 + 5, white);
        }

        SDL_RenderPresent(renderer);

        // Crude frame cap. ~60 FPS is plenty for this tutorial.
        SDL_Delay(16);
    }

    // --- Cleanup --------------------------------------------------------
    if (shootSnd.stream) SDL_DestroyAudioStream(shootSnd.stream);
    if (hitSnd.stream)   SDL_DestroyAudioStream(hitSnd.stream);
    if (crashSnd.stream) SDL_DestroyAudioStream(crashSnd.stream);
    if (shootSnd.buf)    SDL_free(shootSnd.buf);
    if (hitSnd.buf)      SDL_free(hitSnd.buf);
    if (crashSnd.buf)    SDL_free(crashSnd.buf);

    SDL_DestroyTexture(playerTex);
    SDL_DestroyTexture(rabbitTex);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    return 0;
}
