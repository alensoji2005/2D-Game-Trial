#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <SDL2/SDL_mixer.h>

class SoundManager {
public:
    static Mix_Chunk* loadSound(const char* path);
    static Mix_Music* loadMusic(const char* path);
    
    static void playSound(Mix_Chunk* sound);
    static void playMusic(Mix_Music* music);
};

#endif