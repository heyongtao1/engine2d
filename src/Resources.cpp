#include "Resources.hpp"

SDL_Texture* ResourceManager::loadTexture(SDL_Renderer* renderer, const std::string& path) {
    if (textures.find(path) != textures.end()) {
        return textures[path];
    }
    
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image: %s", IMG_GetError());
        return nullptr;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        SDL_Log("Failed to create texture: %s", SDL_GetError());
        return nullptr;
    }
    
    textures[path] = texture;
    return texture;
}

TTF_Font* ResourceManager::loadFont(const std::string& path, int size) {
    std::string key = path + std::to_string(size);
    
    if (fonts.find(key) != fonts.end()) {
        return fonts[key];
    }
    
    TTF_Font* font = TTF_OpenFont(path.c_str(), size);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return nullptr;
    }
    
    fonts[key] = font;
    return font;
}

void ResourceManager::cleanup() {
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
    
    for (auto& pair : fonts) {
        TTF_CloseFont(pair.second);
    }
    fonts.clear();
}