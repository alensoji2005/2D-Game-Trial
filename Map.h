#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include <string>

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();

    // Changed to accept a file path instead of an array
    void LoadMap(std::string path);
    void DrawMap(SDL_Rect camera);
    
    // Method to check if a specific pixel coordinate is a solid obstacle
    bool isSolid(int pixelX, int pixelY);

private:
    SDL_Rect src, dest;
    
    SDL_Texture* dirt;
    SDL_Texture* grass;
    SDL_Texture* water;
    SDL_Renderer* renderer;

    int map[25][30];
};

#endif