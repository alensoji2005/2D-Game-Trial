#include "SoundManager.h"
#include <iostream>

Mix_Chunk* SoundManager::loadSound(const char* path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path);
    if (chunk == NULL) {
        std::cout << "Failed to load sound: " << path << " Error: " << Mix_GetError() << std::endl;
    }
    return chunk;
}

Mix_Music* SoundManager::loadMusic(const char* path) {
    // SDL_mixer can load .wav, .mp3, .ogg depending on your DLLs. .wav and .ogg are safest.
    Mix_Music* music = Mix_LoadMUS(path);
    if (music == NULL) {
        std::cout << "Failed to load music: " << path << " Error: " << Mix_GetError() << std::endl;
    }
    return music;
}

void SoundManager::playSound(Mix_Chunk* sound) {
    // -1 means play on the first available channel, 0 means play exactly once (no looping)
    Mix_PlayChannel(-1, sound, 0); 
}

void SoundManager::playMusic(Mix_Music* music) {
    // -1 means loop the music infinitely
    Mix_PlayMusic(music, -1); 
}