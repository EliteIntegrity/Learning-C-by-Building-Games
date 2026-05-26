/*
 * ============================================================
 *  Musical Fireworks  -  SDL3 Tutorial Project
 * ============================================================
 *
 *  WHAT THE PROGRAM DOES
 *  ---------------------
 *  A continuous firework display plays on screen while a WAV
 *  music file loops in the background.
 *
 *  Every second or so a rocket shoots upward from a random
 *  position at the bottom of the window.  When it reaches its
 *  peak it "explodes" into 80-140 coloured sparks that arc
 *  outward, fall under gravity, and slowly fade away.
 *
 *  A semi-transparent dark rectangle is drawn over the whole
 *  screen each frame instead of clearing it completely.  This
 *  lets old particles linger for a few frames, producing a
 *  glowing streak / motion-blur effect.
 *
 *  AUDIO  (the interesting bit for students)
 *  -----------------------------------------
 *  SDL3 removed SDL_mixer from the core library and replaced
 *  it with a built-in audio streaming API.  The steps are:
 *
 *    1.  SDL_Init(SDL_INIT_AUDIO) -- enable the audio subsystem
 *    2.  SDL_LoadWAV()            -- read a .wav file into RAM
 *    3.  SDL_OpenAudioDevice()    -- open the speaker/headphones
 *    4.  SDL_CreateAudioStream()  -- create a queue that handles
 *                                   sample-rate / format conversion
 *    5.  SDL_BindAudioStream()    -- connect stream to device
 *    6.  SDL_PutAudioStreamData() -- push audio bytes into queue
 *
 *  The device drains the stream automatically in the background.
 *  To loop the music we just re-queue the WAV bytes whenever the
 *  stream runs dry (checked once per frame).
 *
 *  KEY CONTROLS
 *  ------------
 *  Escape / close window  ->  quit
 *
 *  STRUCTS USED
 *  ------------
 *  Particle  - one point of light (rocket body OR explosion spark)
 *  Firework  - a rocket + the vector of sparks it spawns on explosion
 * ============================================================
 */

#include <SDL3/SDL.h>
#include <vector>
#include <algorithm>   // std::remove_if
#include <cstdlib>     // rand, srand
#include <cmath>       // cosf, sinf

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------
const int   WINDOW_W        = 900;
const int   WINDOW_H        = 650;
const int   MAX_FIREWORKS   = 10;
const float GRAVITY         = 90.0f;   // pixels per second^2 (downward = +y)
const float FADE_ALPHA      = 28.0f;   // background fade strength each frame

// ---------------------------------------------------------------------------
//  Structs
// ---------------------------------------------------------------------------

// A single glowing point - used for both the rocket and the explosion sparks
struct Particle {
    float x, y;        // screen position
    float vx, vy;      // velocity in pixels per second
    float life;        // seconds of life remaining
    float maxLife;     // starting life (used to compute alpha fade)
    Uint8 r, g, b;     // colour
};

// One complete firework: a rocket that flies upward, then bursts into sparks
struct Firework {
    Particle            rocket;
    std::vector<Particle> sparks;
    bool                exploded;
};

// ---------------------------------------------------------------------------
//  Helper functions
// ---------------------------------------------------------------------------

// Return a random float in [lo, hi]
float randRange(float lo, float hi) {
    return lo + (hi - lo) * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Build a brand-new rocket aimed at a random height near the top of the screen
Firework spawnFirework() {
    Firework fw;
    fw.exploded       = false;

    fw.rocket.x       = randRange(120.0f, WINDOW_W - 120.0f);
    fw.rocket.y       = static_cast<float>(WINDOW_H);
    fw.rocket.vx      = randRange(-40.0f, 40.0f);
    fw.rocket.vy      = randRange(-420.0f, -250.0f);  // negative = upward
    fw.rocket.life    = 4.0f;
    fw.rocket.maxLife = 4.0f;
    fw.rocket.r = fw.rocket.g = fw.rocket.b = 255;    // white rocket trail

    return fw;
}

// Replace the rocket with a burst of coloured sparks
void explode(Firework& fw) {
    fw.exploded = true;

    // Pick a random vivid colour for this explosion
    Uint8 r = static_cast<Uint8>(randRange(80,  255));
    Uint8 g = static_cast<Uint8>(randRange(80,  255));
    Uint8 b = static_cast<Uint8>(randRange(80,  255));

    int count = 80 + rand() % 70;   // 80 - 149 sparks per burst
    fw.sparks.reserve(count);

    for (int i = 0; i < count; ++i) {
        float angle = randRange(0.0f, 6.28318f);  // 0 to 2*pi
        float speed = randRange(60.0f, 280.0f);

        Particle p;
        p.x       = fw.rocket.x;
        p.y       = fw.rocket.y;
        p.vx      = cosf(angle) * speed;
        p.vy      = sinf(angle) * speed;
        p.life    = randRange(0.7f, 2.2f);
        p.maxLife = p.life;
        p.r = r;  p.g = g;  p.b = b;

        fw.sparks.push_back(p);
    }
}

// Move a particle one time-step forward and age it
void updateParticle(Particle& p, float dt) {
    p.x    += p.vx * dt;
    p.y    += p.vy * dt;
    p.vy   += GRAVITY * dt;   // gravity accelerates downward each frame
    p.life -= dt;
}

// Draw a particle as a tiny filled square; fade alpha with remaining life
void drawParticle(SDL_Renderer* renderer, const Particle& p) {
    Uint8 alpha = static_cast<Uint8>(255.0f * (p.life / p.maxLife));
    SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, alpha);
    SDL_FRect rect = { p.x - 2.0f, p.y - 2.0f, 5.0f, 5.0f };
    SDL_RenderFillRect(renderer, &rect);
}

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------
int main(int /*argc*/, char* /*argv*/[]) {

    // -----------------------------------------------------------------------
    //  1.  Initialise SDL  (both video and audio subsystems)
    // -----------------------------------------------------------------------
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // -----------------------------------------------------------------------
    //  2.  Create a window and a hardware-accelerated 2-D renderer
    // -----------------------------------------------------------------------
    SDL_Window* window = SDL_CreateWindow(
        "Musical Fireworks",
        WINDOW_W, WINDOW_H,
        0                        // no special window flags needed
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);

    if (!window || !renderer) {
        SDL_Log("Window/Renderer failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Allow particles to be drawn with transparency (alpha blending)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // -----------------------------------------------------------------------
    //  3.  Load the WAV file into memory
    //
    //  SDL_LoadWAV reads the file from disk and gives us:
    //    wavSpec    - describes the audio: sample rate, channels, bit depth
    //    wavBuffer  - pointer to the raw PCM audio bytes
    //    wavLength  - how many bytes are in wavBuffer
    //
    //  The buffer is allocated by SDL so we must free it with SDL_free().
    // -----------------------------------------------------------------------
    SDL_AudioSpec wavSpec   = {};
    Uint8*        wavBuffer = nullptr;
    Uint32        wavLength = 0;

    bool audioLoaded = SDL_LoadWAV("Foghorn melody.wav",
                                   &wavSpec,
                                   &wavBuffer,
                                   &wavLength);
    if (!audioLoaded) {
        // Non-fatal: the fireworks will still work, just without music
        SDL_Log("Could not load WAV: %s", SDL_GetError());
    }

    // -----------------------------------------------------------------------
    //  4.  Open the default audio output device (speakers / headphones)
    //
    //  SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK is a special constant that tells
    //  SDL "use whatever the user has set as their default output device".
    //  Passing nullptr for the spec lets SDL pick a suitable hardware format.
    // -----------------------------------------------------------------------
    SDL_AudioDeviceID audioDevice = 0;
    SDL_AudioStream*  audioStream = nullptr;

    if (audioLoaded) {
        audioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                          nullptr);
        if (audioDevice == 0) {
            SDL_Log("Could not open audio device: %s", SDL_GetError());
        }
    }

    // -----------------------------------------------------------------------
    //  5.  Create an audio stream
    //
    //  An SDL3 AudioStream is like a pipe:
    //    - we push raw audio bytes in at the "source" end
    //    - the device pulls converted audio out at the "sink" end
    //
    //  The stream can automatically resample (e.g. 44100 Hz -> 48000 Hz)
    //  and reformat (e.g. 16-bit integer -> 32-bit float) on the fly.
    //
    //  We provide our WAV's format as the source spec.
    //  Passing nullptr as the destination spec means "I'll let the bound
    //  device set the output format automatically".
    // -----------------------------------------------------------------------
    if (audioDevice != 0) {
        audioStream = SDL_CreateAudioStream(&wavSpec, nullptr);
        if (!audioStream) {
            SDL_Log("Could not create audio stream: %s", SDL_GetError());
        }
    }

    // -----------------------------------------------------------------------
    //  6.  Bind the stream to the device and queue the first batch of audio
    //
    //  SDL_BindAudioStream connects the stream to the open device so the
    //  device knows where to pull its audio data from.
    //
    //  SDL_PutAudioStreamData copies our PCM bytes into the stream's
    //  internal queue, ready for the device to consume.
    // -----------------------------------------------------------------------
    if (audioStream) {
        SDL_BindAudioStream(audioDevice, audioStream);

        // Push the entire WAV into the queue for the first playthrough
        SDL_PutAudioStreamData(audioStream,
                               wavBuffer,
                               static_cast<int>(wavLength));
    }

    // -----------------------------------------------------------------------
    //  7.  Set up the fireworks simulation state
    // -----------------------------------------------------------------------
    std::vector<Firework> fireworks;
    fireworks.reserve(MAX_FIREWORKS);

    float launchTimer    = 0.0f;
    float launchInterval = 0.5f;   // seconds between rocket launches

    srand(static_cast<unsigned>(SDL_GetTicks()));   // seed random numbers

    Uint64 lastTick = SDL_GetTicks();

    // -----------------------------------------------------------------------
    //  8.  Main loop
    // -----------------------------------------------------------------------
    bool running = true;
    while (running) {

        // --- Delta time: how many seconds elapsed since the last frame ---
        Uint64 now = SDL_GetTicks();
        float  dt  = static_cast<float>(now - lastTick) / 1000.0f;
        lastTick   = now;
        if (dt > 0.1f) dt = 0.1f;   // cap to avoid huge jumps after pauses

        // --- Process window / keyboard events ---
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            if (event.type == SDL_EVENT_KEY_DOWN &&
                event.key.scancode == SDL_SCANCODE_ESCAPE)
                running = false;
        }

        // --- Audio looping --------------------------------------------------
        //  SDL_GetAudioStreamAvailable returns the number of bytes still
        //  waiting in the stream.  When it reaches 0 the music has finished,
        //  so we push the WAV bytes back in to make it loop seamlessly.
        // -------------------------------------------------------------------
        if (audioStream && wavBuffer) {
            if (SDL_GetAudioStreamAvailable(audioStream) == 0) {
                SDL_PutAudioStreamData(audioStream,
                                       wavBuffer,
                                       static_cast<int>(wavLength));
            }
        }

        // --- Launch a new firework on a timer ---
        launchTimer += dt;
        if (launchTimer >= launchInterval &&
            static_cast<int>(fireworks.size()) < MAX_FIREWORKS) {
            fireworks.push_back(spawnFirework());
            launchTimer    = 0.0f;
            launchInterval = randRange(0.3f, 1.0f);  // randomise next gap
        }

        // --- Update fireworks ---
        for (Firework& fw : fireworks) {
            if (!fw.exploded) {
                updateParticle(fw.rocket, dt);
                // The rocket explodes when it stops climbing (vy flips +ve)
                // or if its lifetime expires
                if (fw.rocket.vy >= 0.0f || fw.rocket.life <= 0.0f)
                    explode(fw);
            } else {
                for (Particle& sp : fw.sparks)
                    if (sp.life > 0.0f)
                        updateParticle(sp, dt);
            }
        }

        // --- Remove fireworks whose every spark has burned out ---
        fireworks.erase(
            std::remove_if(fireworks.begin(), fireworks.end(),
                [](const Firework& fw) {
                    if (!fw.exploded) return false;
                    for (const Particle& p : fw.sparks)
                        if (p.life > 0.0f) return false;
                    return true;
                }),
            fireworks.end()
        );

        // --- Draw -----------------------------------------------------------
        //  Instead of calling SDL_RenderClear (which would wipe to black),
        //  we overlay a nearly-opaque dark rectangle each frame.
        //  Old pixels darken gradually, leaving glowing streaks behind the
        //  particles (the "comet tail" effect).
        // -------------------------------------------------------------------
        SDL_SetRenderDrawColor(renderer, 0, 0, 8,
                               static_cast<Uint8>(FADE_ALPHA));
        SDL_FRect fullscreen = { 0.0f, 0.0f,
                                 static_cast<float>(WINDOW_W),
                                 static_cast<float>(WINDOW_H) };
        SDL_RenderFillRect(renderer, &fullscreen);

        // Draw each firework on top of the faded background
        for (const Firework& fw : fireworks) {
            if (!fw.exploded) {
                drawParticle(renderer, fw.rocket);
            } else {
                for (const Particle& sp : fw.sparks)
                    if (sp.life > 0.0f)
                        drawParticle(renderer, sp);
            }
        }

        // Swap back-buffer to screen
        SDL_RenderPresent(renderer);
    }

    // -----------------------------------------------------------------------
    //  9.  Clean up (in reverse order of creation is good practice)
    // -----------------------------------------------------------------------
    if (audioStream)  SDL_DestroyAudioStream(audioStream);
    if (audioDevice)  SDL_CloseAudioDevice(audioDevice);
    if (wavBuffer)    SDL_free(wavBuffer);    // SDL allocated it, SDL frees it

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
