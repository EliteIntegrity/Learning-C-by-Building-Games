#include "Sound.h"

Sound::~Sound()
{
    if (stream_) SDL_DestroyAudioStream(stream_);
    if (buf_)    SDL_free(buf_);
}

bool Sound::load(const char* path)
{
    SDL_AudioSpec spec;
    if (!SDL_LoadWAV(path, &spec, &buf_, &len_))
    {
        SDL_Log("SDL_LoadWAV failed for %s: %s", path, SDL_GetError());
        return false;
    }

    stream_ = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, nullptr, nullptr);
    if (!stream_)
    {
        SDL_Log("SDL_OpenAudioDeviceStream failed: %s", SDL_GetError());
        SDL_free(buf_);
        buf_ = nullptr;
        return false;
    }

    // Streams start paused; resume so audio actually plays.
    SDL_ResumeAudioStreamDevice(stream_);
    return true;
}

void Sound::play() const
{
    if (!stream_ || !buf_) return;
    // Clear any in-flight audio first so rapid repeats restart cleanly
    // instead of stacking up as a long queue.
    SDL_ClearAudioStream(stream_);
    SDL_PutAudioStreamData(stream_, buf_, (int)len_);
}

void Sounds::loadAll()
{
    // Names match the .wav files expected in the working directory.
    // Missing files are non-fatal — the matching Sound will silently
    // not play. (Drop placeholder .wav files in or comment out a play
    // call if you don't have audio assets handy.)
    hit.load("snd_hit.wav");
    playerHurt.load("snd_hurt.wav");
    kill.load("snd_kill.wav");
    pickup.load("snd_pickup.wav");
    quaff.load("snd_quaff.wav");
    castScroll.load("snd_cast.wav");
    descend.load("snd_descend.wav");
    levelUp.load("snd_levelup.wav");
}
