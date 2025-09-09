#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>
#include <cassert>
#include "Scene.hpp"

class GameEngine;

class SceneManager {
private:
    GameEngine* m_game;
    std::unordered_map<std::string, std::function<std::shared_ptr<Scene>()>> m_sceneFactories;
    std::shared_ptr<Scene> m_currentScene;
    std::string m_currentSceneName;
    std::shared_ptr<Scene> m_nextScene;
    std::string m_nextSceneName;
    
public:
    explicit SceneManager(GameEngine* game);
    
    template<typename T>
    void registerScene(const std::string& sceneName);
    
    void changeScene(const std::string& sceneName, bool endCurrentScene = true);
    void transitionToScene(const std::string& sceneName, float transitionTime = 1.0f);
    
    void update(float deltaTime);
    void doAction(const std::string& actionName);
    
    std::shared_ptr<Scene> getCurrentScene();
    std::string getCurrentSceneName() const;
    
    bool hasScene(const std::string& sceneName) const;
};

// 模板方法实现
template<typename T>
void SceneManager::registerScene(const std::string& sceneName) {
    assert(m_sceneFactories.find(sceneName) == m_sceneFactories.end() && 
           "Scene already registered!");
    
    m_sceneFactories[sceneName] = [this]() {
        return std::make_shared<T>(m_game);
    };
}