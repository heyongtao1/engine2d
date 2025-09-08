#include "Engine.hpp"
#include <iostream>

GameEngine::GameEngine() {
    editor = new Editor(*this);
}

GameEngine::~GameEngine() {
    shutdown();
}

bool GameEngine::initialize(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
    }
    
    window = SDL_CreateWindow(title, 
                             SDL_WINDOWPOS_UNDEFINED, 
                             SDL_WINDOWPOS_UNDEFINED, 
                             width, 
                             height, 
                             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    
    if (!window) {
        std::cerr << "Window could not be created: " << SDL_GetError() << std::endl;
        return false;
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer could not be created: " << SDL_GetError() << std::endl;
        return false;
    }
    
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize: " << IMG_GetError() << std::endl;
        return false;
    }
    
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // 初始化编辑器
    if (!editor->initialize()) {
        std::cerr << "Failed to initialize editor!" << std::endl;
        return false;
    }
    
    renderSystem = new RenderSystem(renderer);
    isRunning = true;
    lastUpdateTime = SDL_GetTicks();
    
    return true;
}

void GameEngine::run() {
    while (isRunning) {
        Uint32 frameStart = SDL_GetTicks();
        
        processInput();
        
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
        lastUpdateTime = currentTime;
        
        update(deltaTime);
        render();
        
        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
}

void GameEngine::shutdown() {
    delete editor;
    editor = nullptr;
    
    delete renderSystem;
    renderSystem = nullptr;
    
    ResourceManager::getInstance().cleanup();
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void GameEngine::processInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        
        // 如果编辑器处于激活状态，不处理游戏输入
        if (editor->isEditorMode()) {

            // 先让编辑器处理事件
            editor->processEvent(event);

            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            continue;
        }

        std::cout << "game engine event type:" << event.type << std::endl;
        
        // 否则，让输入系统处理事件
        inputSystem.update(event, registry);
        if (inputSystem.shouldQuit()) {
            isRunning = false;
        }
    }
}

void GameEngine::update(float deltaTime) {
    // 如果不在编辑器模式，更新游戏系统
    if (!editor->isEditorMode()) {
        movementSystem.update(registry, deltaTime);
        animationSystem.update(registry);
        collisionSystem.update(registry);
        return;
    }
    
    // 更新编辑器
    editor->update(deltaTime);
}

void GameEngine::render() {

    if (!editor->isEditorMode()) {
        // 渲染游戏场景
        if (renderSystem) {
            renderSystem->update(registry);
        }
        return;
    }
    
    // 渲染编辑器
    editor->render();
}

entt::entity GameEngine::createEntity() {
    return registry.create();
}

void GameEngine::destroyEntity(entt::entity entity) {
    registry.destroy(entity);
}