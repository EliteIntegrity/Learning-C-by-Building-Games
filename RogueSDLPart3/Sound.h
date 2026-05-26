#pragma once
#include <SDL3/SDL.h>

// ============================================================
//  Sound  -  one .wav loaded and ready to play.
// ============================================================
//
//  Same pattern as the 10 FX demo. SDL_LoadWAV reads the file
//  into a buffer; we open one audio stream per Sound so each
//  one can be triggered independently without stalling.
// ============================================================

class Sound
{
public:
    Sound() = default;
    ~Sound();

    Sound(const Sound&)            = delete;
    Sound& operator=(const Sound&) = delete;

    bool load(const char* path);
    void play() const;

    bool ok() const { return stream_ != nullptr; }

private:
    SDL_AudioStream* stream_ = nullptr;
    Uint8*           buf_    = nullptr;
    Uint32           len_    = 0;
};

// Bundle of every sound the game uses. The Game owns one of these.
struct Sounds
{
    Sound hit;
    Sound playerHurt;
    Sound kill;
    Sound pickup;
    Sound quaff;
    Sound castScroll;
    Sound descend;
    Sound levelUp;

    void loadAll();
};
