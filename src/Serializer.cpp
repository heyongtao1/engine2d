#include "Serializer.hpp"
#include "Scene.hpp"
#include "Resources.hpp"
// 组件序列化
nlohmann::json ComponentSerializer::positionComponentSerializer(const Position* position)
{
    assert(position);
    return {{"x", position->x}, {"y", position->y}};
}

nlohmann::json ComponentSerializer::velocityComponentSerializer(const Velocity* velocity)
{
    assert(velocity);
    return {{"dx", velocity->dx}, {"dy", velocity->dy}};
}

nlohmann::json ComponentSerializer::spriteComponentSerializer(const Sprite* sprite)
{
    assert(sprite);
    return {
        {"texturePath", sprite->texturePath},
        {"srcRect", {{"x", sprite->srcRect.x}, {"y", sprite->srcRect.y}, {"w", sprite->srcRect.w}, {"h", sprite->srcRect.h}}},
        {"dstRect", {{"x", sprite->dstRect.x}, {"y", sprite->dstRect.y}, {"w", sprite->dstRect.w}, {"h", sprite->dstRect.h}}},
        {"visible", sprite->visible}
    };
}

nlohmann::json ComponentSerializer::colliderComponentSerializer(const Collider* collider)
{
    assert(collider);
    return {
        {"rect", {{"x", collider->rect.x}, {"y", collider->rect.y}, {"w", collider->rect.w}, {"h", collider->rect.h}}},
        {"isTrigger", collider->isTrigger}
        // 注意：不序列化collidedWith，因为它是运行时状态
    };
}

nlohmann::json ComponentSerializer::tagComponentSerializer(const Tag* tag)
{
    assert(tag);
    return {{"name", tag->name}};
}

nlohmann::json ComponentSerializer::playerControlComponentSerializer(const PlayerControl* playerControl)
{
    assert(playerControl);
    return {{"speed", playerControl->speed}};
}

// 组件反序列化
Position ComponentSerializer::positionComponentDerializer(const nlohmann::json& data)
{
    Position position;
    position.x = data["x"];
    position.y = data["y"];
    return position;
}

Velocity ComponentSerializer::velocityComponentDerializer(const nlohmann::json& data)
{
    Velocity velocity;
    velocity.dx = data["dx"];
    velocity.dy = data["dy"];
    return velocity;
}

Sprite ComponentSerializer::spriteComponentDerializer(const nlohmann::json& data)
{
    Sprite sprite;
    sprite.texturePath = data["texturePath"];
    sprite.srcRect.x = data["srcRect"]["x"];
    sprite.srcRect.y = data["srcRect"]["y"];
    sprite.srcRect.h = data["srcRect"]["h"];
    sprite.srcRect.w = data["srcRect"]["w"];
    sprite.dstRect.x = data["dstRect"]["x"];
    sprite.dstRect.y = data["dstRect"]["y"];
    sprite.dstRect.h = data["dstRect"]["h"];
    sprite.dstRect.w = data["dstRect"]["w"];
    sprite.visible = data["visible"];
    return sprite;
}

Collider ComponentSerializer::colliderComponentDerializer(const nlohmann::json& data)
{
    Collider collider;
    collider.rect.x = data["rect"]["x"];
    collider.rect.y = data["rect"]["y"];
    collider.rect.h = data["rect"]["h"];
    collider.rect.w = data["rect"]["w"];
    collider.isTrigger = data["isTrigger"];
    return collider;
}

Tag ComponentSerializer::tagComponentDerializer(const nlohmann::json& data)
{
    Tag tag;
    tag.name = data["name"];
    return tag;
}

PlayerControl ComponentSerializer::playerControlComponentDerializer(const nlohmann::json& data)
{
    PlayerControl playerControl;
    playerControl.speed = data["speed"];
    return playerControl;
}

// 实体序列化
nlohmann::json EntitySerializer::serializer(entt::registry& registry, entt::entity et)
{
    nlohmann::json entityJson;
    entityJson["id"] = static_cast<uint32_t>(et);
    
    // 序列化每个组件
    if (registry.all_of<Position>(et)) {
        entityJson["components"]["Position"] = ComponentSerializer::positionComponentSerializer(&registry.get<Position>(et));
    }
    
    if (registry.all_of<Velocity>(et)) {
        entityJson["components"]["Velocity"] = ComponentSerializer::velocityComponentSerializer(&registry.get<Velocity>(et));
    }
    
    if (registry.all_of<Sprite>(et)) {
        entityJson["components"]["Sprite"] = ComponentSerializer::spriteComponentSerializer(&registry.get<Sprite>(et));
    }
    
    if (registry.all_of<Tag>(et)) {
        entityJson["components"]["Tag"] = ComponentSerializer::tagComponentSerializer(&registry.get<Tag>(et));
    }
    
    if (registry.all_of<Collider>(et)) {
        entityJson["components"]["Collider"] = ComponentSerializer::colliderComponentSerializer(&registry.get<Collider>(et));
    }
    
    if (registry.all_of<PlayerControl>(et)) {
        entityJson["components"]["PlayerControl"] = ComponentSerializer::playerControlComponentSerializer(&registry.get<PlayerControl>(et));
    }

    return entityJson;
}

void EntitySerializer::derializer(const nlohmann::json& data, entt::registry& registry)
{
    entt::entity et = registry.create();

    // 反序列化每个组件
    if (data["components"].contains("Position")) {
        registry.emplace<Position>(et, 
            ComponentSerializer::positionComponentDerializer(data["components"]["Position"]));
    }

    if (data["components"].contains("Velocity")) {
        registry.emplace<Velocity>(et, 
            ComponentSerializer::velocityComponentDerializer(data["components"]["Velocity"]));
    }

    if (data["components"].contains("Sprite")) {
        Sprite sprite = ComponentSerializer::spriteComponentDerializer(data["components"]["Sprite"]);
        // 加载纹理
        if (!sprite.texturePath.empty()) {
            sprite.texture = ResourceManager::getInstance().loadTexture(
                SDL_GL_GetCurrentWindow() ? 
                SDL_GetRenderer(SDL_GL_GetCurrentWindow()) : nullptr,
                sprite.texturePath
            );
        }
        registry.emplace<Sprite>(et, sprite);
    }

    if (data["components"].contains("Collider")) {
        registry.emplace<Collider>(et, 
            ComponentSerializer::colliderComponentDerializer(data["components"]["Collider"]));
    }

    if (data["components"].contains("Tag")) {
        registry.emplace<Tag>(et, 
            ComponentSerializer::tagComponentDerializer(data["components"]["Tag"]));
    }

    if (data["components"].contains("PlayerControl")) {
        registry.emplace<PlayerControl>(et, 
            ComponentSerializer::playerControlComponentDerializer(data["components"]["PlayerControl"]));
    }
}

// 场景序列化
nlohmann::json SceneSerializer::serializer(Scene* scene)
{
    assert(scene);
    nlohmann::json sceneJson;
    entt::registry& registry = scene->getRegistry();

    // 收集所有实体
    auto view = registry.view<entt::entity>();
    nlohmann::json entitiesJson = nlohmann::json::array();
    
    for (auto entity : view) {
        entitiesJson.push_back(EntitySerializer::serializer(registry, entity));
    }

    sceneJson["entities"] = entitiesJson;
    sceneJson["metadata"] = {
        {"version", "1.0"},
        {"entityCount", entitiesJson.size()}
    };

    return sceneJson;
}

void SceneSerializer::derializer(const nlohmann::json& data, Scene* scene)
{
    assert(scene);

    entt::registry& registry = scene->getRegistry();
    registry.clear();

    // 创建实体
    for (const auto& entityJson : data["entities"]) {
        EntitySerializer::derializer(entityJson, registry);
    }
}