#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

// 资源管理器
class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }
    
    SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
    TTF_Font* loadFont(const std::string& path, int size);
    
    void cleanup();
    
private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, TTF_Font*> fonts;
};