#pragma once
#include <entt/entt.hpp>
#include <SDL.h>
#include <string>
#include <memory>

class GameEngine; // 前向声明

class Scene {
protected:
    GameEngine* m_game;
    entt::registry m_registry;
    bool m_paused;
    bool m_hasBegun;
    
public:
    explicit Scene(GameEngine* gameEngine);
    virtual ~Scene();
    
    // 禁止拷贝和赋值
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    
    // 虚函数接口 - 必须在派生类中实现
    virtual void update(float deltaTime) = 0;
    virtual void doAction(const std::string& actionName) = 0;
    
    // 可选实现的虚函数
    virtual void onBegin();
    virtual void onEnd();
    virtual void sDoAction(const std::string& actionName);
    
    // 通用功能
    void setPaused(bool paused);
    entt::registry& getRegistry();
    GameEngine* game();
    
    // 模拟Unity风格的函数，便于使用
    virtual void start() {};
    virtual void awake() {};
};