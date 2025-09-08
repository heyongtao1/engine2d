#pragma once

#include <SDL.h>
#include <string>
#include <entt/entt.hpp>

// 位置组件
struct Position {
    float x, y;
};

// 速度组件
struct Velocity {
    float dx, dy;
};

// 精灵组件
struct Sprite {
    SDL_Texture* texture = nullptr;
    SDL_Rect srcRect;
    SDL_Rect dstRect;
    bool visible = true;
};

// 动画组件
struct Animation {
    int frameCount;
    int currentFrame;
    int frameDelay;
    int frameWidth;
    int frameHeight;
    int animationSpeed;
    Uint32 lastUpdate;
};

// 碰撞组件
struct Collider {
    SDL_Rect rect;
    bool isTrigger = false;
    entt::entity collidedWith = entt::null;
};

// 标签组件
struct Tag {
    std::string name;
};

// 玩家控制组件
struct PlayerControl {
    float speed = 5.0f;
};