#include "AudioManager.h"
#include <map>

AudioManager::AudioManager()
    : mNextEffectSlot(0)
{
}

void AudioManager::playMusic(const std::string& path, float volume) {
    if (mCurrentTrack == path) return; // already playing — do nothing

    if (mMusic.openFromFile(path)) {
        mMusic.setLoop(true);
        mMusic.setVolume(volume);
        mMusic.play();
        mCurrentTrack = path;
    }
}

void AudioManager::stopMusic() {
    mMusic.stop();
    mCurrentTrack.clear();
}

void AudioManager::playEffect(const std::string& path, float volume) {
    // cache each sound buffer after its first load — avoids reloading
    // the same short clip from disk every single time it's triggered
    static sf::SoundBuffer buffers[16];
    static std::string loadedPaths[16];
    static int bufferCount = 0;

    int bufferIndex = -1;
    for (int i = 0; i < bufferCount; i++) {
        if (loadedPaths[i] == path) { bufferIndex = i; break; }
    }

    if (bufferIndex == -1) {
        if (bufferCount >= 16) return; // pool full — silently skip
        if (!buffers[bufferCount].loadFromFile(path)) return;
        loadedPaths[bufferCount] = path;
        bufferIndex = bufferCount;
        bufferCount++;
    }

    mEffectPool[mNextEffectSlot].setBuffer(buffers[bufferIndex]);
    mEffectPool[mNextEffectSlot].setVolume(volume);
    mEffectPool[mNextEffectSlot].play();
    mNextEffectSlot = (mNextEffectSlot + 1) % EFFECT_POOL_SIZE;
}