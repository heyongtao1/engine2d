#pragma once

#include <entt/entt.hpp>
#include <SDL.h>

// 输入系统
class InputSystem {
public:
    void update(entt::registry& registry);
    
    bool isKeyDown(SDL_Scancode key) const;
    bool shouldQuit() const;
    
private:
    bool keys[SDL_NUM_SCANCODES] = {false};
    bool quit = false;
};

// 运动系统
class MovementSystem {
public:
    void update(entt::registry& registry, float deltaTime);
};

// 动画系统
class AnimationSystem {
public:
    void update(entt::registry& registry);
};

// 渲染系统
class RenderSystem {
public:
    RenderSystem(SDL_Renderer* renderer) : renderer(renderer) {}
    void update(entt::registry& registry);
    
private:
    SDL_Renderer* renderer;
};

// 碰撞系统
class CollisionSystem {
public:
    void update(entt::registry& registry);
};