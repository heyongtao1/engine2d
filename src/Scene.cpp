#include "Scene.hpp"
#include "Engine.hpp"

Scene::Scene(GameEngine* gameEngine) 
    : m_game(gameEngine), m_paused(false), m_hasBegun(false) {}

Scene::~Scene() {
    // 清理所有实体
    m_registry.clear();
}

void Scene::onBegin() {
    if (!m_hasBegun) {
        awake();
        start();
        m_hasBegun = true;
    }
}

void Scene::onEnd() {
    m_hasBegun = false;
}

void Scene::sDoAction(const std::string& actionName) {
    // 默认实现为空，派生类可以重写
}

void Scene::setPaused(bool paused) {
    m_paused = paused;
}

entt::registry& Scene::getRegistry() {
    return m_registry;
}

GameEngine* Scene::game() {
    return m_game;
}