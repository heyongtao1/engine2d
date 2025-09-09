#include "SceneManager.hpp"
#include "Engine.hpp"
#include <iostream>

SceneManager::SceneManager(GameEngine* game) 
    : m_game(game), m_currentScene(nullptr) {}

void SceneManager::changeScene(const std::string& sceneName, bool endCurrentScene) {
    // 检查场景是否已注册
    if (m_sceneFactories.find(sceneName) == m_sceneFactories.end()) {
        std::cerr << "Attempted to change to unknown scene: " << sceneName << std::endl;
        return;
    }
    
    // 结束当前场景
    if (m_currentScene && endCurrentScene) {
        m_currentScene->onEnd();
    }
    
    // 创建新场景
    m_currentScene = m_sceneFactories[sceneName]();
    m_currentSceneName = sceneName;
    
    // 开始新场景
    m_currentScene->onBegin();
    
    std::cout << "Changed to scene: " << sceneName << std::endl;
}

void SceneManager::transitionToScene(const std::string& sceneName, float transitionTime) {
    // 简化实现：直接切换场景
    // 在实际引擎中，这里可以实现淡入淡出等过渡效果
    changeScene(sceneName);
}

void SceneManager::update(float deltaTime) {
    if (m_currentScene) {
        m_currentScene->update(deltaTime);
    }
}

void SceneManager::doAction(const std::string& actionName) {
    if (m_currentScene) {
        m_currentScene->doAction(actionName);
    }
}

std::shared_ptr<Scene> SceneManager::getCurrentScene() {
    return m_currentScene;
}

std::string SceneManager::getCurrentSceneName() const {
    return m_currentSceneName;
}

bool SceneManager::hasScene(const std::string& sceneName) const {
    return m_sceneFactories.find(sceneName) != m_sceneFactories.end();
}