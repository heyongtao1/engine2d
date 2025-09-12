#pragma once

#include <entt/entt.hpp>
#include <SDL.h>
#include "imgui.h"
#include <map>
#include <functional>

class GameEngine;

class Editor {
public:
    Editor(GameEngine& engine);
    ~Editor();
    
    bool initialize();
    void processEvent(SDL_Event& event);
    void update(float deltaTime);
    void render();
    
    bool isEditorMode() const { return editorMode; }
    void setEditorMode(bool mode) { editorMode = mode; }
    
private:
    void renderMainMenu();
    void renderEntityList();
    void renderComponentEditor(entt::entity entity);
    void renderSceneView();
    void renderInspector();
    void renderResourceView();
    void renderStats();
    void renderCreateEntityPopup();
    void renderAddComponentPopup();
    
    void createEmptyScene();
    void createEntity();
    void addComponentToEntity(entt::entity entity, const std::string& componentType);
    void handleEntityDeletion(entt::entity entity);
    
    GameEngine& engine;
    entt::registry& registry;
    SDL_Renderer* renderer;
    
    bool editorMode = true;
    bool showDemoWindow = false;
    bool showEntityList = true;
    bool showInspector = true;
    bool showSceneView = true;
    bool showResourceView = false;
    bool showStats = true;
    bool showCreateEntityPopup = false;
    bool showAddComponentPopup = false;
    
    entt::entity selectedEntity = entt::null;
    
    // 场景视图相关
    ImVec2 sceneViewSize = ImVec2(800, 600);
    ImVec2 sceneViewPos = ImVec2(0, 0);
    float sceneZoom = 1.0f;
    ImVec2 scenePan = ImVec2(0, 0);
    bool isSceneViewHovered = false;
    
    // 资源浏览器相关
    char assetPathBuffer[256] = "assets/";
    
    // 组件类型映射
    std::map<std::string, std::function<void(entt::entity)>> componentCreators;
};