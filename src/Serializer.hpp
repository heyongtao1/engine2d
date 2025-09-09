#pragma once

#include "Components.hpp"
#include <nlohmann/json.hpp>

class Scene;

class ComponentSerializer
{
public:
    // 组件序列化
    static nlohmann::json positionComponentSerializer(const Position* position);

    static nlohmann::json velocityComponentSerializer(const Velocity* velocity);

    static nlohmann::json spriteComponentSerializer(const Sprite* sprite);

    static nlohmann::json colliderComponentSerializer(const Collider* collider);

    static nlohmann::json tagComponentSerializer(const Tag* tag);

    static nlohmann::json playerControlComponentSerializer(const PlayerControl* playerControl);

    // 组件反序列化
    static Position positionComponentDerializer(const nlohmann::json& data);

    static Velocity velocityComponentDerializer(const nlohmann::json& data);

    static Sprite spriteComponentDerializer(const nlohmann::json& data);

    static Collider colliderComponentDerializer(const nlohmann::json& data);

    static Tag tagComponentDerializer(const nlohmann::json& data);

    static PlayerControl playerControlComponentDerializer(const nlohmann::json& data);
};

class EntitySerializer
{
public:
    // 实体序列化
    static nlohmann::json serializer(entt::registry& registry, entt::entity et);
    // 实体反序列化
    static void derializer(const nlohmann::json& data, entt::registry& registry);
};

class SceneSerializer
{
public:
    // 场景序列化
    static nlohmann::json serializer(Scene* scene);
    // 场景反序列化
    static void derializer(const nlohmann::json& data, Scene* scene);
};