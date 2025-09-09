#pragma once

#include <entt/entt.hpp>
#include <SDL.h>
#include "Systems.hpp"
#include "Resources.hpp"
#include "Editor.hpp"
#include "SceneManager.hpp"

class GameEngine {
public:
    GameEngine();
    ~GameEngine();
    
    bool initialize(const char* title, int width, int height);
    void run();
    void shutdown();
    
    entt::registry& getRegistry() { return registry; }
    SDL_Renderer* getRenderer() { return renderer; }
    SDL_Window* getWindow() { return window; }
    
    entt::entity createEntity();
    void destroyEntity(entt::entity entity);
    
    Editor& getEditor() { return *editor; }

    SceneManager& getSceneManager() { return *m_sceneManager; }
    
private:
    void processInput();
    void update(float deltaTime);
    void render();
    
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    entt::registry registry;
    InputSystem inputSystem;
    MovementSystem movementSystem;
    AnimationSystem animationSystem;
    RenderSystem* renderSystem = nullptr;
    CollisionSystem collisionSystem;
    
    Editor* editor = nullptr;
    
    bool isRunning = false;
    Uint32 lastUpdateTime = 0;

    std::unique_ptr<SceneManager> m_sceneManager;
    
    const int FPS = 60;
    const int FRAME_DELAY = 1000 / FPS;
};