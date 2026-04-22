#ifndef MAP_H
#define MAP_H

#include <SDL2/SDL.h>
#include <string>

class Map {
public:
    Map(SDL_Renderer* renderer);
    ~Map();

    void LoadMap(std::string path);
    void DrawMap(SDL_Rect camera);
    
    bool isSolid(int pixelX, int pixelY);

private:
    SDL_Rect src, dest;
    
    // --- NEW: We only need ONE texture pointer now! ---
    SDL_Texture* tileset; 
    SDL_Renderer* renderer;

    int map[25][30];
};

#endif