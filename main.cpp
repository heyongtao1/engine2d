#include "Engine.hpp"
#include "Components.hpp"
#include "Resources.hpp"

int main(int argc, char* argv[]) {
    GameEngine engine;
    
    if (!engine.initialize("ECS Game Engine with Editor", 1280, 720)) {
        return -1;
    }
    
    // 创建一些示例实体
    auto player = engine.createEntity();
    auto& playerPos = engine.getRegistry().emplace<Position>(player, 400.0f, 300.0f);
    auto& playerVel = engine.getRegistry().emplace<Velocity>(player);
    auto& playerControl = engine.getRegistry().emplace<PlayerControl>(player);
    auto& playerTag = engine.getRegistry().emplace<Tag>(player, "Player");
    
    // 加载玩家精灵
    SDL_Texture* playerTexture = ResourceManager::getInstance().loadTexture(
        engine.getRenderer(), "assets/player.png");
    
    if (playerTexture) {
        int w, h;
        SDL_QueryTexture(playerTexture, nullptr, nullptr, &w, &h);
        
        auto& sprite = engine.getRegistry().emplace<Sprite>(player);
        sprite.texture = playerTexture;
        sprite.srcRect = {0, 0, w, h};
        sprite.dstRect = {static_cast<int>(playerPos.x), static_cast<int>(playerPos.y), w, h};
    }
    
    // 添加碰撞体
    auto& collider = engine.getRegistry().emplace<Collider>(player);
    collider.rect = {static_cast<int>(playerPos.x), static_cast<int>(playerPos.y), 32, 32};
    
    // 创建一些测试实体
    for (int i = 0; i < 5; ++i) {
        auto enemy = engine.createEntity();
        auto& enemyPos = engine.getRegistry().emplace<Position>(enemy, 
            static_cast<float>(100 + i * 100), 
            static_cast<float>(100 + i * 50));
        
        auto& enemyVel = engine.getRegistry().emplace<Velocity>(enemy);
        enemyVel.dx = (i % 2 == 0) ? 1.0f : -1.0f;
        enemyVel.dy = (i % 3 == 0) ? 1.0f : -1.0f;
        
        auto& enemyTag = engine.getRegistry().emplace<Tag>(enemy, "Enemy " + std::to_string(i));
        
        // 加载敌人精灵
        SDL_Texture* enemyTexture = ResourceManager::getInstance().loadTexture(
            engine.getRenderer(), "assets/enemy.png");
        
        if (enemyTexture) {
            int w, h;
            SDL_QueryTexture(enemyTexture, nullptr, nullptr, &w, &h);
            
            auto& sprite = engine.getRegistry().emplace<Sprite>(enemy);
            sprite.texture = enemyTexture;
            sprite.srcRect = {0, 0, w, h};
            sprite.dstRect = {static_cast<int>(enemyPos.x), static_cast<int>(enemyPos.y), w, h};
        }
        
        // 添加碰撞体
        auto& enemyCollider = engine.getRegistry().emplace<Collider>(enemy);
        enemyCollider.rect = {static_cast<int>(enemyPos.x), static_cast<int>(enemyPos.y), 32, 32};
    }
    
    // 运行游戏
    engine.run();
    
    return 0;
}