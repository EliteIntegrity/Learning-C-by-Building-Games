#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/*
    10 FX - SDL3 Beginner Sound FX Demo

    Features:
    - Opens an SDL3 window with 10 colored pads (one per sound)
    - Press keys 0-9 (top row or numpad) to play the matching .wav file
    - The corresponding pad lights up briefly when triggered
    - Escape or closing the window quits
    - Each sound can be re-triggered while playing (clears and restarts)

    How it works:
    - SDL_LoadWAV loads all 10 files (0.wav - 9.wav) at startup
    - One SDL_AudioStream per sound is created and bound to the default audio device
    - On a key press, SDL_ClearAudioStream + SDL_PutAudioStreamData queues the audio
      SDL3 mixes and plays it automatically
    - The pad highlight uses a 120ms timer via SDL_GetTicks

    Note: .wav files must be in the working directory.
    In VS: Project Properties -> Debugging -> Working Directory -> $(ProjectDir)
*/

struct Sound {
    SDL_AudioSpec spec;
    Uint8* buf = nullptr;
    Uint32 len = 0;
};

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("10 FX - Press keys 0-9 to play sounds", 415, 130, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!window || !renderer) {
        SDL_Log("Window/Renderer creation failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Sound sounds[10];
    for (int i = 0; i < 10; i++) {
        char path[16];
        SDL_snprintf(path, sizeof(path), "%d.wav", i);
        if (!SDL_LoadWAV(path, &sounds[i].spec, &sounds[i].buf, &sounds[i].len))
            SDL_Log("Could not load %s: %s", path, SDL_GetError());
    }

    SDL_AudioDeviceID device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!device) {
        SDL_Log("SDL_OpenAudioDevice failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_AudioStream* streams[10] = {};
    for (int i = 0; i < 10; i++) {
        if (sounds[i].buf) {
            streams[i] = SDL_CreateAudioStream(&sounds[i].spec, nullptr);
            if (streams[i])
                SDL_BindAudioStream(device, streams[i]);
        }
    }

    // Colors for each pad (one per sound)
    static const SDL_FColor padColors[10] = {
        {1.0f, 0.3f, 0.3f, 1.0f},
        {1.0f, 0.6f, 0.2f, 1.0f},
        {1.0f, 1.0f, 0.2f, 1.0f},
        {0.4f, 1.0f, 0.3f, 1.0f},
        {0.2f, 0.8f, 1.0f, 1.0f},
        {0.3f, 0.4f, 1.0f, 1.0f},
        {0.7f, 0.3f, 1.0f, 1.0f},
        {1.0f, 0.3f, 0.8f, 1.0f},
        {0.9f, 0.6f, 0.4f, 1.0f},
        {0.5f, 0.9f, 0.7f, 1.0f},
    };

    int active = -1;           // which pad is lit up
    Uint64 activeUntil = 0;    // timestamp (ms) when to stop lighting it

    SDL_Log("Ready! Press keys 0-9 (or numpad) to play sounds.");

    bool running = true;
    SDL_Event ev;
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT)
                running = false;
            else if (ev.type == SDL_EVENT_KEY_DOWN && !ev.key.repeat) {
                if (ev.key.key == SDLK_ESCAPE)
                    running = false;

                int idx = -1;
                if (ev.key.key >= SDLK_0 && ev.key.key <= SDLK_9)
                    idx = (int)(ev.key.key - SDLK_0);
                else if (ev.key.key >= SDLK_KP_0 && ev.key.key <= SDLK_KP_9)
                    idx = (int)(ev.key.key - SDLK_KP_0);

                if (idx >= 0 && streams[idx]) {
                    SDL_ClearAudioStream(streams[idx]);
                    SDL_PutAudioStreamData(streams[idx], sounds[idx].buf, (int)sounds[idx].len);
                    active = idx;
                    activeUntil = SDL_GetTicks() + 120;
                }
            }
        }

        if (active >= 0 && SDL_GetTicks() > activeUntil)
            active = -1;

        // Draw pads
        SDL_SetRenderDrawColorFloat(renderer, 0.12f, 0.12f, 0.12f, 1.0f);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 10; i++) {
            SDL_FRect rect = { i * 41.0f + 5.0f, 15.0f, 36.0f, 100.0f };
            float bright = (i == active) ? 1.0f : 0.45f;
            SDL_SetRenderDrawColorFloat(renderer,
                padColors[i].r * bright,
                padColors[i].g * bright,
                padColors[i].b * bright,
                1.0f);
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    for (int i = 0; i < 10; i++) {
        SDL_DestroyAudioStream(streams[i]);
        SDL_free(sounds[i].buf);
    }
    SDL_CloseAudioDevice(device);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
