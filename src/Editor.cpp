#include "Editor.hpp"
#include "Engine.hpp"
#include "Components.hpp"
#include "Resources.hpp"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <algorithm>
#include <string>
#include <iostream>

Editor::Editor(GameEngine& engine) 
    : engine(engine), registry(engine.getRegistry()) {
    // 初始化组件创建器映射
    componentCreators["Position"] = [this](entt::entity entity) {
        registry.emplace<Position>(entity, 0.0f, 0.0f);
    };
    
    componentCreators["Velocity"] = [this](entt::entity entity) {
        registry.emplace<Velocity>(entity, 0.0f, 0.0f);
    };
    
    componentCreators["Sprite"] = [this](entt::entity entity) {
        registry.emplace<Sprite>(entity);
    };
    
    componentCreators["Collider"] = [this](entt::entity entity) {
        registry.emplace<Collider>(entity);
    };
    
    componentCreators["Tag"] = [this](entt::entity entity) {
        registry.emplace<Tag>(entity, "New Entity");
    };
    
    componentCreators["PlayerControl"] = [this](entt::entity entity) {
        registry.emplace<PlayerControl>(entity);
    };
}

Editor::~Editor() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

bool Editor::initialize() {
    // 设置ImGui上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    renderer = engine.getRenderer(); // 在这里获取渲染器

    // 检查渲染器是否有效
    if (!renderer) {
        std::cerr << "Renderer is null in Editor::initialize()!" << std::endl;
        return false;
    }

    // 设置ImGui样式
    ImGui::StyleColorsDark();
    
    // 初始化SDL2后端
    if (!ImGui_ImplSDL2_InitForSDLRenderer(engine.getWindow(), renderer)) {
        return false;
    }
    
    if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
        return false;
    }
    
    return true;
}

void Editor::processEvent(SDL_Event& event) {
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void Editor::update(float deltaTime) {
    // 开始ImGui帧
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // 创建停靠空间
    ImGui::DockSpaceOverViewport(0);
    
    // 渲染主菜单
    renderMainMenu();
    
    // 渲染各个窗口
    if (showEntityList) renderEntityList();
    if (showInspector) renderInspector();
    if (showSceneView) renderSceneView();
    if (showResourceView) renderResourceView();
    if (showStats) renderStats();
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    renderCreateEntityPopup();
    renderAddComponentPopup();
}

void Editor::render() {
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
}

void Editor::renderMainMenu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene")) {
                createEmptyScene();
            }
            if (ImGui::MenuItem("Save Scene")) {
                // 保存场景逻辑
            }
            if (ImGui::MenuItem("Load Scene")) {
                // 加载场景逻辑
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                engine.shutdown();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Create Entity")) {
                showCreateEntityPopup = true;
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Entity List", NULL, &showEntityList);
            ImGui::MenuItem("Inspector", NULL, &showInspector);
            ImGui::MenuItem("Scene View", NULL, &showSceneView);
            ImGui::MenuItem("Resource View", NULL, &showResourceView);
            ImGui::MenuItem("Stats", NULL, &showStats);
            ImGui::MenuItem("ImGui Demo", NULL, &showDemoWindow);
            ImGui::EndMenu();
        }
        
        // 编辑器模式切换
        ImGui::SameLine(ImGui::GetWindowWidth() - 120);
        if (ImGui::Button(editorMode ? "Play" : "Stop")) {
            editorMode = !editorMode;
        }
        
        ImGui::EndMainMenuBar();
    }
}

void Editor::renderEntityList() {
    ImGui::Begin("Entity List", &showEntityList);
    
    // 实体列表
    auto view = registry.view<Tag>();
    for (auto entity : view) {
        const auto& tag = view.get<Tag>(entity);
        bool isSelected = (selectedEntity == entity);
        
        if (ImGui::Selectable(tag.name.c_str(), isSelected)) {
            selectedEntity = entity;
        }
        
        // 右键菜单
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
                handleEntityDeletion(entity);
                if (selectedEntity == entity) {
                    selectedEntity = entt::null;
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    
    // 添加实体按钮
    if (ImGui::Button("+ Create Entity")) {
        showCreateEntityPopup = true;
    }
    
    ImGui::End();
}

void Editor::renderCreateEntityPopup() {
    // 首先检查是否需要打开弹出窗口
    if (showCreateEntityPopup) {
        ImGui::OpenPopup("Create Entity");
        showCreateEntityPopup = false; // 重置标志，避免重复打开
    }
    
    // 设置弹出窗口的大小
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    
    // 开始模态弹出窗口
    if (ImGui::BeginPopupModal("Create Entity", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char entityName[128] = "New Entity";
        
        ImGui::Text("Create a new entity:");
        ImGui::Separator();
        
        ImGui::InputText("Name", entityName, IM_ARRAYSIZE(entityName));
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // 按钮布局
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            createEntity();
            if (registry.all_of<Tag>(selectedEntity)) {
                auto& tag = registry.get<Tag>(selectedEntity);
                tag.name = entityName;
            }
            // 重置输入框
            strcpy(entityName, "New Entity");
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            // 重置输入框
            strcpy(entityName, "New Entity");
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void Editor::renderAddComponentPopup() {
    // 首先检查是否需要打开弹出窗口
    if (showAddComponentPopup) {
        ImGui::OpenPopup("Add Component");
        showAddComponentPopup = false; // 重置标志，避免重复打开
    }
    
    // 设置弹出窗口的大小
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    
    // 开始模态弹出窗口
    if (ImGui::BeginPopupModal("Add Component", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Select component type to add:");
        ImGui::Separator();
        
        // 列出所有可用的组件类型
        for (const auto& [name, creator] : componentCreators) {
            // 检查实体是否已经有这个组件
            bool hasComponent = false;
            if (name == "Position") hasComponent = registry.all_of<Position>(selectedEntity);
            else if (name == "Velocity") hasComponent = registry.all_of<Velocity>(selectedEntity);
            else if (name == "Sprite") hasComponent = registry.all_of<Sprite>(selectedEntity);
            else if (name == "Collider") hasComponent = registry.all_of<Collider>(selectedEntity);
            else if (name == "Tag") hasComponent = registry.all_of<Tag>(selectedEntity);
            else if (name == "PlayerControl") hasComponent = registry.all_of<PlayerControl>(selectedEntity);
            
            // 如果实体还没有这个组件，显示添加按钮
            if (!hasComponent) {
                if (ImGui::Button(name.c_str(), ImVec2(-1, 0))) {
                    addComponentToEntity(selectedEntity, name);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::Spacing();
            }
        }
        
        ImGui::Separator();
        
        if (ImGui::Button("Cancel", ImVec2(-1, 0))) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
}

void Editor::renderInspector() {
    ImGui::Begin("Inspector", &showInspector);
    
    if (selectedEntity != entt::null && registry.valid(selectedEntity)) {
        // 显示和编辑标签组件
        if (registry.all_of<Tag>(selectedEntity)) {
            auto& tag = registry.get<Tag>(selectedEntity);
            char nameBuffer[256];
            strcpy(nameBuffer, tag.name.c_str());
            
            if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
                tag.name = std::string(nameBuffer);
            }
        } else {
            if (ImGui::Button("Add Tag Component")) {
                registry.emplace<Tag>(selectedEntity, "Entity");
            }
        }
        
        ImGui::Separator();
        
        // 显示和编辑位置组件
        if (registry.all_of<Position>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen)) {
                auto& position = registry.get<Position>(selectedEntity);
                ImGui::DragFloat("X", &position.x, 1.0f);
                ImGui::DragFloat("Y", &position.y, 1.0f);
            }
        }
        
        // 显示和编辑速度组件
        if (registry.all_of<Velocity>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Velocity")) {
                auto& velocity = registry.get<Velocity>(selectedEntity);
                ImGui::DragFloat("dX", &velocity.dx, 0.1f);
                ImGui::DragFloat("dY", &velocity.dy, 0.1f);
            }
        }
        
        // 显示和编辑精灵组件
        if (registry.all_of<Sprite>(selectedEntity)) {
            if (ImGui::CollapsingHeader("Sprite")) {
                auto& sprite = registry.get<Sprite>(selectedEntity);
                ImGui::Text("Texture: %s", sprite.texturePath.c_str());
                ImGui::Checkbox("Visible", &sprite.visible);
                
                // 纹理选择器
                if (ImGui::Button("Change Texture")) {
                    // 打开纹理选择对话框
                }
            }
        }
        
        // 添加组件按钮
        ImGui::Separator();
        if (ImGui::Button("Add Component")) {
            showAddComponentPopup = true;
        }
    } else {
        ImGui::Text("No entity selected");
    }
    
    ImGui::End();
}

void Editor::createEmptyScene() {
    registry.clear();
    selectedEntity = entt::null;
    std::cout << "Created empty scene" << std::endl;
}

void Editor::createEntity() {
    selectedEntity = registry.create();
    registry.emplace<Tag>(selectedEntity, "New Entity");
    registry.emplace<Position>(selectedEntity, 0.0f, 0.0f);
    std::cout << "Created new entity: " << static_cast<uint32_t>(selectedEntity) << std::endl;
}

void Editor::addComponentToEntity(entt::entity entity, const std::string& componentType) {
    auto it = componentCreators.find(componentType);
    if (it != componentCreators.end()) {
        it->second(entity);
        std::cout << "Added " << componentType << " component to entity " 
                  << static_cast<uint32_t>(entity) << std::endl;
    }
}

void Editor::renderSceneView() {
    ImGui::Begin("Scene View", &showSceneView);
    
    isSceneViewHovered = ImGui::IsWindowHovered();
    sceneViewPos = ImGui::GetCursorScreenPos();
    sceneViewSize = ImGui::GetContentRegionAvail();
    
    // 创建场景渲染目标
    ImGui::Image(
        (ImTextureID)(intptr_t)renderer, 
        sceneViewSize, 
        ImVec2(0, 0), 
        ImVec2(1, 1), 
        ImVec4(1, 1, 1, 1), 
        ImVec4(0.5f, 0.5f, 0.5f, 1.0f)
    );
    
    // 场景导航控制
    if (isSceneViewHovered) {
        // 缩放
        float zoomFactor = ImGui::GetIO().MouseWheel * 0.1f;
        if (zoomFactor != 0.0f) {
            sceneZoom += zoomFactor;
            sceneZoom = std::clamp(sceneZoom, 0.1f, 5.0f);
        }
        
        // 平移
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            scenePan.x += ImGui::GetIO().MouseDelta.x;
            scenePan.y += ImGui::GetIO().MouseDelta.y;
        }
    }
    
    // 在场景视图中选择实体
    if (isSceneViewHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 relativePos = ImVec2(
            (mousePos.x - sceneViewPos.x) / sceneZoom - scenePan.x,
            (mousePos.y - sceneViewPos.y) / sceneZoom - scenePan.y
        );
        
        // 简单的点选检测
        auto view = registry.view<Position, Sprite>();
        for (auto entity : view) {
            const auto& position = view.get<Position>(entity);
            const auto& sprite = view.get<Sprite>(entity);
            
            if (relativePos.x >= position.x && 
                relativePos.x <= position.x + sprite.dstRect.w &&
                relativePos.y >= position.y && 
                relativePos.y <= position.y + sprite.dstRect.h) {
                selectedEntity = entity;
                break;
            }
        }
    }
    
    ImGui::End();
}

void Editor::renderResourceView() {
    ImGui::Begin("Resource View", &showResourceView);
    
    ImGui::InputText("Asset Path", assetPathBuffer, sizeof(assetPathBuffer));
    
    if (ImGui::Button("Load Texture")) {
        // 加载纹理逻辑
    }
    
    // 显示已加载的纹理
    ImGui::Separator();
    ImGui::Text("Loaded Textures:");
    
    // 这里需要访问ResourceManager的纹理列表
    // 为了简化，我们假设有一个获取纹理列表的方法
    
    ImGui::End();
}

void Editor::renderStats() {
    ImGui::Begin("Stats", &showStats);
    
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    //ImGui::Text("Entities: %zu", registry.size());
    
    // ImGui::Separator();
    // ImGui::Text("Components:");
    // ImGui::Text("  Position: %zu", registry.size<Position>());
    // ImGui::Text("  Velocity: %zu", registry.size<Velocity>());
    // ImGui::Text("  Sprite: %zu", registry.size<Sprite>());
    // ImGui::Text("  Collider: %zu", registry.size<Collider>());
    
    ImGui::End();
}

void Editor::handleEntityDeletion(entt::entity entity) {
    engine.destroyEntity(entity);
}