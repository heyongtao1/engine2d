#include "Systems.hpp"
#include "Components.hpp"

void InputSystem::update(entt::registry& registry) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
        
        if (event.type == SDL_KEYDOWN) {
            keys[event.key.keysym.scancode] = true;
        }
        
        if (event.type == SDL_KEYUP) {
            keys[event.key.keysym.scancode] = false;
        }
    }
    
    // 处理玩家输入
    auto view = registry.view<PlayerControl, Velocity>();
    for (auto entity : view) {
        auto& velocity = view.get<Velocity>(entity);
        velocity.dx = 0;
        velocity.dy = 0;
        
        if (isKeyDown(SDL_SCANCODE_W)) velocity.dy = -10;
        if (isKeyDown(SDL_SCANCODE_S)) velocity.dy = 10;
        if (isKeyDown(SDL_SCANCODE_A)) velocity.dx = -10;
        if (isKeyDown(SDL_SCANCODE_D)) velocity.dx = 10;
        
        // 标准化对角线移动
        if (velocity.dx != 0 && velocity.dy != 0) {
            velocity.dx *= 0.7071f; // 1/sqrt(2)
            velocity.dy *= 0.7071f;
        }
        
        // 应用速度
        const auto& control = view.get<PlayerControl>(entity);
        velocity.dx *= control.speed;
        velocity.dy *= control.speed;
    }
}

bool InputSystem::isKeyDown(SDL_Scancode key) const {
    return keys[key];
}

bool InputSystem::shouldQuit() const {
    return quit;
}

void MovementSystem::update(entt::registry& registry, float deltaTime) {
    auto view = registry.view<Position, Velocity>();
    for (auto entity : view) {
        auto& position = view.get<Position>(entity);
        const auto& velocity = view.get<Velocity>(entity);
        
        position.x += velocity.dx * deltaTime;
        position.y += velocity.dy * deltaTime;
        
        // 更新精灵位置
        if (registry.all_of<Sprite>(entity)) {
            auto& sprite = registry.get<Sprite>(entity);
            sprite.dstRect.x = static_cast<int>(position.x);
            sprite.dstRect.y = static_cast<int>(position.y);
        }
        
        // 更新碰撞体位置
        if (registry.all_of<Collider>(entity)) {
            auto& collider = registry.get<Collider>(entity);
            collider.rect.x = static_cast<int>(position.x);
            collider.rect.y = static_cast<int>(position.y);
        }
    }
}

void AnimationSystem::update(entt::registry& registry) {
    Uint32 currentTime = SDL_GetTicks();
    
    auto view = registry.view<Animation, Sprite>();
    for (auto entity : view) {
        auto& animation = view.get<Animation>(entity);
        auto& sprite = view.get<Sprite>(entity);
        
        if (currentTime - animation.lastUpdate > animation.animationSpeed) {
            animation.currentFrame = (animation.currentFrame + 1) % animation.frameCount;
            sprite.srcRect.x = animation.currentFrame * animation.frameWidth;
            animation.lastUpdate = currentTime;
        }
    }
}

void RenderSystem::update(entt::registry& registry) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    auto view = registry.view<Sprite, Position>();
    for (auto entity : view) {
        const auto& sprite = view.get<Sprite>(entity);
        if (sprite.visible) {
            SDL_RenderCopy(renderer, sprite.texture, &sprite.srcRect, &sprite.dstRect);
        }
    }
    
    // 调试渲染：显示碰撞体
    auto colliderView = registry.view<Collider, Position>();
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (auto entity : colliderView) {
        const auto& collider = colliderView.get<Collider>(entity);
        SDL_RenderDrawRect(renderer, &collider.rect);
    }
    
    SDL_RenderPresent(renderer);
}

void CollisionSystem::update(entt::registry& registry) {
    auto view = registry.view<Collider, Position>();
    
    for (auto entity1 : view) {
        const auto& collider1 = view.get<Collider>(entity1);
        const auto& pos1 = view.get<Position>(entity1);
        
        for (auto entity2 : view) {
            if (entity1 == entity2) continue;
            
            const auto& collider2 = view.get<Collider>(entity2);
            const auto& pos2 = view.get<Position>(entity2);
            
            if (SDL_HasIntersection(&collider1.rect, &collider2.rect)) {
                // 处理碰撞
                if (collider1.isTrigger || collider2.isTrigger) {
                    // 触发事件
                    registry.patch<Collider>(entity1, [entity2](auto& col) {
                        col.collidedWith = entity2;
                    });
                } else {
                    // 物理碰撞响应
                    // 这里可以实现更复杂的物理响应
                }
            }
        }
    }
}