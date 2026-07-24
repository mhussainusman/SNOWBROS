
#pragma once
#include <SFML/Audio.hpp>
#include <string>

// AudioManager: one background music track playing at a time (looped),
// plus a small pool of one-shot sound effects for later use
// (snowball throw, enemy kick, rocket hit, etc.)
class AudioManager {
public:
    AudioManager();

    // switches background music — does nothing if this track is
    // already the one playing, so calling it every frame is safe
    void playMusic(const std::string& path, float volume = 60.f);
    void stopMusic();

    // one-shot sound effects — call this any time you want a sound
    // to play once (e.g. AudioManager::playEffect("assets/Sounds/throw.ogg"))
    // caches each buffer after its first load, same static-caching
    // pattern already used elsewhere in this project
    void playEffect(const std::string& path, float volume = 80.f);

private:
    sf::Music mMusic;
    std::string mCurrentTrack;   // path of whatever's playing right now

    // small fixed pool of Sound objects so multiple effects can
    // overlap (e.g. two snowballs thrown close together)
    static const int EFFECT_POOL_SIZE = 8;
    sf::Sound mEffectPool[EFFECT_POOL_SIZE];
    int mNextEffectSlot;
};